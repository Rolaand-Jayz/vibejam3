#include "project_repository.hpp"

#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTemporaryDir>
#include <QtTest/QtTest>

class ProjectRepositoryTest : public QObject {
    Q_OBJECT

private slots:
    void create_persists_intake_state_across_reopen();
    void update_persists_manifest_changes_across_reopen();
    void create_builds_project_workspace_layout();
    void legacy_aggregate_store_is_migrated_forward();
    void intake_validation_creates_persisted_job_and_report();
    void intake_validation_history_survives_reopen();
    void blueprint_creation_persists_and_advances_state();
    void blueprint_versions_are_sorted_newest_first();
};

void ProjectRepositoryTest::create_persists_intake_state_across_reopen()
{
    QTemporaryDir tempDir;
    QVERIFY2(tempDir.isValid(), "temporary directory should be available");

    ProjectRepository repository(tempDir.path());
    ProjectDraft draft {
        .gameTitle = QStringLiteral("Final Fantasy X-2"),
        .platform = QStringLiteral("PS2"),
        .guideIntent = QStringLiteral("complete walkthrough"),
        .playerGoal = QStringLiteral("100% completion"),
        .runStyle = QStringLiteral("optimized"),
        .spoilerPolicy = QStringLiteral("minimal"),
        .depthPreference = QStringLiteral("completionist"),
        .sharedThemeCss = QStringLiteral(".guide-theme-surface { --accent: #f0b35a; }"),
    };

    Project created;
    QString errorMessage;
    QVERIFY2(repository.create(draft, &created, &errorMessage), qPrintable(errorMessage));
    QCOMPARE(created.projectState, QStringLiteral("intake"));

    ProjectRepository reopened(tempDir.path());
    const QVector<Project> projects = reopened.all(&errorMessage);
    QVERIFY2(errorMessage.isEmpty(), qPrintable(errorMessage));
    QCOMPARE(projects.size(), 1);
    QCOMPARE(projects.front().gameTitle, QStringLiteral("Final Fantasy X-2"));
    QCOMPARE(projects.front().projectState, QStringLiteral("intake"));
    QCOMPARE(projects.front().sharedThemeCss, QStringLiteral(".guide-theme-surface { --accent: #f0b35a; }"));
}

void ProjectRepositoryTest::update_persists_manifest_changes_across_reopen()
{
    QTemporaryDir tempDir;
    QVERIFY2(tempDir.isValid(), "temporary directory should be available");

    ProjectRepository repository(tempDir.path());
    ProjectDraft original {
        .gameTitle = QStringLiteral("Dark Souls"),
        .platform = QStringLiteral("PC"),
        .guideIntent = QStringLiteral("boss route"),
        .playerGoal = QStringLiteral("finish the main path"),
        .runStyle = QStringLiteral("casual"),
        .spoilerPolicy = QStringLiteral("moderate"),
        .depthPreference = QStringLiteral("standard"),
        .sharedThemeCss = QStringLiteral(".guide-theme-surface { --panel-bg: rgba(28, 26, 24, 0.92); }"),
    };

    Project created;
    QString errorMessage;
    QVERIFY2(repository.create(original, &created, &errorMessage), qPrintable(errorMessage));

    ProjectDraft revised {
        .gameTitle = QStringLiteral("Dark Souls Remastered"),
        .platform = QStringLiteral("Switch"),
        .guideIntent = QStringLiteral("all major bosses"),
        .playerGoal = QStringLiteral("collect key equipment"),
        .runStyle = QStringLiteral("optimized"),
        .spoilerPolicy = QStringLiteral("full"),
        .depthPreference = QStringLiteral("comprehensive"),
        .sharedThemeCss = QStringLiteral(".guide-theme-surface { --accent: #b97745; }"),
    };

    Project updated;
    QVERIFY2(repository.update(created.id, revised, &updated, &errorMessage), qPrintable(errorMessage));
    QCOMPARE(updated.id, created.id);
    QCOMPARE(updated.projectState, QStringLiteral("intake"));
    QCOMPARE(updated.gameTitle, QStringLiteral("Dark Souls Remastered"));
    QCOMPARE(updated.platform, QStringLiteral("Switch"));

    ProjectRepository reopened(tempDir.path());
    const QVector<Project> projects = reopened.all(&errorMessage);
    QVERIFY2(errorMessage.isEmpty(), qPrintable(errorMessage));
    QCOMPARE(projects.size(), 1);
    QCOMPARE(projects.front().gameTitle, QStringLiteral("Dark Souls Remastered"));
    QCOMPARE(projects.front().platform, QStringLiteral("Switch"));
    QCOMPARE(projects.front().guideIntent, QStringLiteral("all major bosses"));
    QCOMPARE(projects.front().projectState, QStringLiteral("intake"));
    QCOMPARE(projects.front().sharedThemeCss, QStringLiteral(".guide-theme-surface { --accent: #b97745; }"));
}

void ProjectRepositoryTest::create_builds_project_workspace_layout()
{
    QTemporaryDir tempDir;
    QVERIFY2(tempDir.isValid(), "temporary directory should be available");

    ProjectRepository repository(tempDir.path());
    ProjectDraft draft {
        .gameTitle = QStringLiteral("Metroid Prime"),
        .platform = QStringLiteral("GameCube"),
        .guideIntent = QStringLiteral("sequence break notes"),
        .playerGoal = QStringLiteral("collect key upgrades"),
        .runStyle = QStringLiteral("challenge"),
        .spoilerPolicy = QStringLiteral("moderate"),
        .depthPreference = QStringLiteral("comprehensive"),
        .sharedThemeCss = QStringLiteral(".guide-theme-surface { --text-primary: #f1efe9; }"),
    };

    Project created;
    QString errorMessage;
    QVERIFY2(repository.create(draft, &created, &errorMessage), qPrintable(errorMessage));

    const QString projectRoot = tempDir.path() + QStringLiteral("/projects/") + created.id;
    QVERIFY2(QDir(projectRoot).exists(), qPrintable(projectRoot));
    QVERIFY2(QFile::exists(projectRoot + QStringLiteral("/project-manifest.json")), "manifest file should exist");
    QVERIFY2(QDir(projectRoot + QStringLiteral("/editions")).exists(), "editions directory should exist");
    QVERIFY2(QDir(projectRoot + QStringLiteral("/previews")).exists(), "previews directory should exist");
    QVERIFY2(QDir(projectRoot + QStringLiteral("/assets")).exists(), "assets directory should exist");
    QVERIFY2(QDir(projectRoot + QStringLiteral("/validations")).exists(), "validations directory should exist");
    QVERIFY2(QDir(projectRoot + QStringLiteral("/exports")).exists(), "exports directory should exist");
}

void ProjectRepositoryTest::legacy_aggregate_store_is_migrated_forward()
{
    QTemporaryDir tempDir;
    QVERIFY2(tempDir.isValid(), "temporary directory should be available");

    const QString legacyFilePath = tempDir.path() + QStringLiteral("/projects.json");
    QFile legacyFile(legacyFilePath);
    QVERIFY2(legacyFile.open(QIODevice::WriteOnly | QIODevice::Text), "legacy projects file should open");

    QJsonArray legacyProjects;
    legacyProjects.push_back(QJsonObject {
        { QStringLiteral("id"), QStringLiteral("legacy-project-1") },
        { QStringLiteral("gameTitle"), QStringLiteral("Vagrant Story") },
        { QStringLiteral("platform"), QStringLiteral("PS1") },
        { QStringLiteral("guideIntent"), QStringLiteral("boss strategy notebook") },
        { QStringLiteral("playerGoal"), QStringLiteral("finish the story") },
        { QStringLiteral("runStyle"), QStringLiteral("casual") },
        { QStringLiteral("spoilerPolicy"), QStringLiteral("minimal") },
        { QStringLiteral("depthPreference"), QStringLiteral("standard") },
        { QStringLiteral("createdAt"), QStringLiteral("2026-04-22T00:00:00Z") },
    });
    legacyFile.write(QJsonDocument(legacyProjects).toJson(QJsonDocument::Indented));
    legacyFile.close();

    ProjectRepository repository(tempDir.path());
    QString errorMessage;
    const QVector<Project> projects = repository.all(&errorMessage);
    QVERIFY2(errorMessage.isEmpty(), qPrintable(errorMessage));
    QCOMPARE(projects.size(), 1);
    QCOMPARE(projects.front().id, QStringLiteral("legacy-project-1"));
    QCOMPARE(projects.front().projectState, QStringLiteral("intake"));
    QVERIFY2(QFile::exists(tempDir.path() + QStringLiteral("/projects/legacy-project-1/project-manifest.json")), "legacy project should be migrated to workspace layout");
    QVERIFY2(QFile::exists(tempDir.path() + QStringLiteral("/projects.legacy-imported.json")), "legacy file should be preserved as an import backup");
    QVERIFY2(!QFile::exists(legacyFilePath), "legacy aggregate store should no longer remain in the active location");
}

void ProjectRepositoryTest::intake_validation_creates_persisted_job_and_report()
{
    QTemporaryDir tempDir;
    QVERIFY2(tempDir.isValid(), "temporary directory should be available");

    ProjectRepository repository(tempDir.path());
    ProjectDraft draft {
        .gameTitle = QStringLiteral("Dragon Quest VIII"),
        .platform = QStringLiteral(""),
        .guideIntent = QStringLiteral("starter route"),
        .playerGoal = QStringLiteral(""),
        .runStyle = QStringLiteral("casual"),
        .spoilerPolicy = QStringLiteral("minimal"),
        .depthPreference = QStringLiteral("standard"),
        .sharedThemeCss = QStringLiteral(".guide-theme-surface { --panel-border: rgba(240, 179, 90, 0.35); }"),
    };

    Project created;
    QString errorMessage;
    QVERIFY2(repository.create(draft, &created, &errorMessage), qPrintable(errorMessage));
    QVERIFY2(repository.runIntakeValidation(created.id, &errorMessage), qPrintable(errorMessage));

    const auto jobs = repository.listJobs(created.id, &errorMessage);
    QVERIFY2(errorMessage.isEmpty(), qPrintable(errorMessage));
    QCOMPARE(jobs.size(), 1);
    QCOMPARE(jobs.front().jobType, QStringLiteral("validate"));
    QCOMPARE(jobs.front().status, QStringLiteral("blocked"));
    QCOMPARE(jobs.front().progress, 100);

    const auto reports = repository.listValidationReports(created.id, &errorMessage);
    QVERIFY2(errorMessage.isEmpty(), qPrintable(errorMessage));
    QCOMPARE(reports.size(), 1);
    QCOMPARE(reports.front().blockingCount, 1);
    QCOMPARE(reports.front().warningCount, 1);
    QVERIFY2(QFile::exists(tempDir.path() + QStringLiteral("/projects/") + created.id + QStringLiteral("/jobs/") + jobs.front().id + QStringLiteral(".json")), "job record should be written to the project workspace");
    QVERIFY2(QFile::exists(tempDir.path() + QStringLiteral("/projects/") + created.id + QStringLiteral("/validations/") + reports.front().id + QStringLiteral(".json")), "validation report should be written to the project workspace");
}

void ProjectRepositoryTest::intake_validation_history_survives_reopen()
{
    QTemporaryDir tempDir;
    QVERIFY2(tempDir.isValid(), "temporary directory should be available");

    ProjectRepository repository(tempDir.path());
    ProjectDraft draft {
        .gameTitle = QStringLiteral("Persona 4 Golden"),
        .platform = QStringLiteral("PC"),
        .guideIntent = QStringLiteral("social link route"),
        .playerGoal = QStringLiteral("max core links"),
        .runStyle = QStringLiteral("optimized"),
        .spoilerPolicy = QStringLiteral("moderate"),
        .depthPreference = QStringLiteral("comprehensive"),
        .sharedThemeCss = QStringLiteral(".guide-theme-surface { --accent: #d7a44c; }"),
    };

    Project created;
    QString errorMessage;
    QVERIFY2(repository.create(draft, &created, &errorMessage), qPrintable(errorMessage));
    QVERIFY2(repository.runIntakeValidation(created.id, &errorMessage), qPrintable(errorMessage));

    ProjectRepository reopened(tempDir.path());
    const auto jobs = reopened.listJobs(created.id, &errorMessage);
    QVERIFY2(errorMessage.isEmpty(), qPrintable(errorMessage));
    QCOMPARE(jobs.size(), 1);
    QCOMPARE(jobs.front().status, QStringLiteral("completed"));

    const auto reports = reopened.listValidationReports(created.id, &errorMessage);
    QVERIFY2(errorMessage.isEmpty(), qPrintable(errorMessage));
    QCOMPARE(reports.size(), 1);
    QCOMPARE(reports.front().blockingCount, 0);
    QCOMPARE(reports.front().warningCount, 0);
}

void ProjectRepositoryTest::blueprint_creation_persists_and_advances_state()
{
    QTemporaryDir tempDir;
    QVERIFY2(tempDir.isValid(), "temporary directory should be available");

    ProjectRepository repository(tempDir.path());
    ProjectDraft draft {
        .gameTitle = QStringLiteral("Chrono Cross"),
        .platform = QStringLiteral("PS1"),
        .guideIntent = QStringLiteral("true ending route"),
        .playerGoal = QStringLiteral("recruit key characters"),
        .runStyle = QStringLiteral("optimized"),
        .spoilerPolicy = QStringLiteral("moderate"),
        .depthPreference = QStringLiteral("comprehensive"),
        .sharedThemeCss = QStringLiteral(".guide-theme-surface { --accent: #d4ab68; }"),
    };

    Project created;
    QString errorMessage;
    QVERIFY2(repository.create(draft, &created, &errorMessage), qPrintable(errorMessage));

    BlueprintDraft blueprintDraft {
        .chapterPlans = {
            BlueprintChapterPlan { QStringLiteral("Opening route"), QStringLiteral("Set expectations and early missables.") },
            BlueprintChapterPlan { QStringLiteral("Branch logic"), QStringLiteral("Explain recruit splits and route commitments.") },
        },
        .terminologyRules = QStringLiteral("Use official in-game names for locations and systems."),
        .styleBible = QStringLiteral("Keep tactical guidance compact, calm, and spoiler-aware."),
        .crossReferencePlan = QStringLiteral("Link recruit notes back to route commitment callouts."),
        .checklistPlan = QStringLiteral("End each chapter with irreversible checks."),
        .visualPackIntent = QStringLiteral("Warm field-manual cards with restrained emphasis."),
        .spoilerGuardrails = QStringLiteral("Avoid late-game identity reveals in headings."),
    };

    EditorialBlueprint createdBlueprint;
    QVERIFY2(repository.createBlueprint(created.id, blueprintDraft, &createdBlueprint, &errorMessage), qPrintable(errorMessage));
    QCOMPARE(createdBlueprint.version, 1);
    QCOMPARE(createdBlueprint.chapterPlans.size(), 2);

    ProjectRepository reopened(tempDir.path());
    const QVector<Project> projects = reopened.all(&errorMessage);
    QVERIFY2(errorMessage.isEmpty(), qPrintable(errorMessage));
    QCOMPARE(projects.size(), 1);
    QCOMPARE(projects.front().projectState, QStringLiteral("blueprint-ready"));

    const QVector<EditorialBlueprint> blueprints = reopened.listBlueprints(created.id, &errorMessage);
    QVERIFY2(errorMessage.isEmpty(), qPrintable(errorMessage));
    QCOMPARE(blueprints.size(), 1);
    QCOMPARE(blueprints.front().version, 1);
    QCOMPARE(blueprints.front().chapterPlans.front().title, QStringLiteral("Opening route"));
    QVERIFY2(QFile::exists(tempDir.path() + QStringLiteral("/projects/") + created.id + QStringLiteral("/blueprints/") + createdBlueprint.id + QStringLiteral(".json")), "blueprint record should be written to the project workspace");
}

void ProjectRepositoryTest::blueprint_versions_are_sorted_newest_first()
{
    QTemporaryDir tempDir;
    QVERIFY2(tempDir.isValid(), "temporary directory should be available");

    ProjectRepository repository(tempDir.path());
    ProjectDraft draft {
        .gameTitle = QStringLiteral("Vagrant Story"),
        .platform = QStringLiteral("PS1"),
        .guideIntent = QStringLiteral("boss strategy notebook"),
        .playerGoal = QStringLiteral("finish the story"),
        .runStyle = QStringLiteral("casual"),
        .spoilerPolicy = QStringLiteral("minimal"),
        .depthPreference = QStringLiteral("standard"),
        .sharedThemeCss = QStringLiteral(),
    };

    Project created;
    QString errorMessage;
    QVERIFY2(repository.create(draft, &created, &errorMessage), qPrintable(errorMessage));

    BlueprintDraft firstDraft {
        .chapterPlans = {
            BlueprintChapterPlan { QStringLiteral("Workshop opening"), QStringLiteral("Cover first systems and weapon basics.") },
        },
        .terminologyRules = QStringLiteral("Prefer official boss names."),
        .styleBible = QStringLiteral("Use short tactical paragraphs."),
        .crossReferencePlan = QStringLiteral("Tie material notes back to weapon families."),
        .checklistPlan = QStringLiteral("Track workshop exits and key pickups."),
        .visualPackIntent = QStringLiteral("Dense reference cards with low ornament."),
        .spoilerGuardrails = QStringLiteral("Keep late dungeon names out of early summaries."),
    };

    BlueprintDraft secondDraft {
        .chapterPlans = {
            BlueprintChapterPlan { QStringLiteral("Workshop opening"), QStringLiteral("Cover first systems and weapon basics.") },
            BlueprintChapterPlan { QStringLiteral("Catacombs loop"), QStringLiteral("Explain optional detours and risk/reward.") },
        },
        .terminologyRules = QStringLiteral("Prefer official boss names and weapon class labels."),
        .styleBible = QStringLiteral("Use short tactical paragraphs with one caution callout per chapter."),
        .crossReferencePlan = QStringLiteral("Tie material notes back to weapon families and break arts."),
        .checklistPlan = QStringLiteral("Track workshop exits, key pickups, and affinity prep."),
        .visualPackIntent = QStringLiteral("Dense reference cards with low ornament and warmer highlights."),
        .spoilerGuardrails = QStringLiteral("Keep late dungeon names out of early summaries."),
    };

    EditorialBlueprint firstBlueprint;
    QVERIFY2(repository.createBlueprint(created.id, firstDraft, &firstBlueprint, &errorMessage), qPrintable(errorMessage));

    EditorialBlueprint secondBlueprint;
    QVERIFY2(repository.createBlueprint(created.id, secondDraft, &secondBlueprint, &errorMessage), qPrintable(errorMessage));

    const QVector<EditorialBlueprint> blueprints = repository.listBlueprints(created.id, &errorMessage);
    QVERIFY2(errorMessage.isEmpty(), qPrintable(errorMessage));
    QCOMPARE(blueprints.size(), 2);
    QCOMPARE(blueprints.front().version, 2);
    QCOMPARE(blueprints.back().version, 1);
    QCOMPARE(blueprints.front().chapterPlans.size(), 2);
}

QTEST_MAIN(ProjectRepositoryTest)
#include "project_repository_test.moc"

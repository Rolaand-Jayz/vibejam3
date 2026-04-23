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
    void preview_generation_requires_a_blueprint();
    void preview_generation_persists_and_advances_state();
    void preview_review_decisions_persist_and_regeneration_versions();
    void full_build_requires_an_approved_preview();
    void full_build_persists_bundle_and_freeze_pending_state();
    void full_build_with_blocking_visible_conflict_sets_validation_failed_state();
    void knowledge_entities_require_provenance_and_persist_hidden_codex_entries();
    void knowledge_entity_updates_survive_reopen_without_regressing_preview_state();
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
    QVERIFY2(QDir(projectRoot + QStringLiteral("/knowledge")).exists(), "knowledge directory should exist");
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

void ProjectRepositoryTest::preview_generation_requires_a_blueprint()
{
    QTemporaryDir tempDir;
    QVERIFY2(tempDir.isValid(), "temporary directory should be available");

    ProjectRepository repository(tempDir.path());
    ProjectDraft draft {
        .gameTitle = QStringLiteral("Final Fantasy VI"),
        .platform = QStringLiteral("SNES"),
        .guideIntent = QStringLiteral("balanced route"),
        .playerGoal = QStringLiteral("finish the story"),
        .runStyle = QStringLiteral("casual"),
        .spoilerPolicy = QStringLiteral("minimal"),
        .depthPreference = QStringLiteral("standard"),
        .sharedThemeCss = QStringLiteral(),
    };

    Project created;
    QString errorMessage;
    QVERIFY2(repository.create(draft, &created, &errorMessage), qPrintable(errorMessage));

    PreviewPackage createdPreview;
    QVERIFY2(!repository.createPreview(created.id, &createdPreview, &errorMessage), "preview generation should fail without a blueprint");
    QVERIFY2(errorMessage.contains(QStringLiteral("blueprint"), Qt::CaseInsensitive), qPrintable(errorMessage));
}

void ProjectRepositoryTest::preview_generation_persists_and_advances_state()
{
    QTemporaryDir tempDir;
    QVERIFY2(tempDir.isValid(), "temporary directory should be available");

    ProjectRepository repository(tempDir.path());
    ProjectDraft draft {
        .gameTitle = QStringLiteral("Chrono Trigger"),
        .platform = QStringLiteral("SNES"),
        .guideIntent = QStringLiteral("boss order and hidden loot route"),
        .playerGoal = QStringLiteral("see every ending"),
        .runStyle = QStringLiteral("casual"),
        .spoilerPolicy = QStringLiteral("minimal"),
        .depthPreference = QStringLiteral("standard"),
        .sharedThemeCss = QStringLiteral(".guide-theme-surface { --accent: #d2a25a; }"),
    };

    Project created;
    QString errorMessage;
    QVERIFY2(repository.create(draft, &created, &errorMessage), qPrintable(errorMessage));

    BlueprintDraft blueprintDraft {
        .chapterPlans = {
            BlueprintChapterPlan { QStringLiteral("Opening route"), QStringLiteral("Establish the first route locks and early warnings.") },
            BlueprintChapterPlan { QStringLiteral("Core progression"), QStringLiteral("Lay out the main route, power spikes, and detours.") },
            BlueprintChapterPlan { QStringLiteral("Cleanup"), QStringLiteral("Track missables and ending requirements.") },
        },
        .terminologyRules = QStringLiteral("Use official boss, location, and item names."),
        .styleBible = QStringLiteral("Keep the tone calm, tactical, and spoiler-aware."),
        .crossReferencePlan = QStringLiteral("Tie route warnings back to optional detours and reference notes."),
        .checklistPlan = QStringLiteral("End each chapter with a short irreversible-check list."),
        .visualPackIntent = QStringLiteral("Warm field-manual cards with restrained accents."),
        .spoilerGuardrails = QStringLiteral("Keep plot reveals out of headings and summaries."),
    };

    EditorialBlueprint blueprint;
    QVERIFY2(repository.createBlueprint(created.id, blueprintDraft, &blueprint, &errorMessage), qPrintable(errorMessage));

    PreviewPackage createdPreview;
    QVERIFY2(repository.createPreview(created.id, &createdPreview, &errorMessage), qPrintable(errorMessage));
    QCOMPARE(createdPreview.version, 1);
    QCOMPARE(createdPreview.blueprintVersion, 1);
    QCOMPARE(createdPreview.approvalState, QStringLiteral("requires-review"));
    QVERIFY2(createdPreview.sampleSections.size() >= 4, "preview should include multiple representative sample sections");

    ProjectRepository reopened(tempDir.path());
    const QVector<Project> projects = reopened.all(&errorMessage);
    QVERIFY2(errorMessage.isEmpty(), qPrintable(errorMessage));
    QCOMPARE(projects.size(), 1);
    QCOMPARE(projects.front().projectState, QStringLiteral("preview-ready"));

    const QVector<PreviewPackage> previews = reopened.listPreviews(created.id, &errorMessage);
    QVERIFY2(errorMessage.isEmpty(), qPrintable(errorMessage));
    QCOMPARE(previews.size(), 1);
    QCOMPARE(previews.front().approvalState, QStringLiteral("requires-review"));
    QVERIFY2(QFile::exists(tempDir.path() + QStringLiteral("/projects/") + created.id + QStringLiteral("/previews/") + createdPreview.id + QStringLiteral(".json")), "preview record should be written to the project workspace");
}

void ProjectRepositoryTest::preview_review_decisions_persist_and_regeneration_versions()
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

    BlueprintDraft blueprintDraft {
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

    EditorialBlueprint blueprint;
    QVERIFY2(repository.createBlueprint(created.id, blueprintDraft, &blueprint, &errorMessage), qPrintable(errorMessage));

    PreviewPackage firstPreview;
    QVERIFY2(repository.createPreview(created.id, &firstPreview, &errorMessage), qPrintable(errorMessage));

    PreviewPackage changesRequestedPreview;
    QVERIFY2(repository.reviewPreview(firstPreview.id, QStringLiteral("changes-requested"), QStringLiteral("The route sample needs denser checkpoint language before full build."), &changesRequestedPreview, &errorMessage), qPrintable(errorMessage));
    QCOMPARE(changesRequestedPreview.approvalState, QStringLiteral("changes-requested"));
    QVERIFY2(changesRequestedPreview.reviewNotes.contains(QStringLiteral("checkpoint language"), Qt::CaseInsensitive), "review notes should be preserved");

    const QVector<Project> revisionRequestedProjects = repository.all(&errorMessage);
    QVERIFY2(errorMessage.isEmpty(), qPrintable(errorMessage));
    QCOMPARE(revisionRequestedProjects.front().projectState, QStringLiteral("revision-requested"));

    PreviewPackage secondPreview;
    QVERIFY2(repository.createPreview(created.id, &secondPreview, &errorMessage), qPrintable(errorMessage));
    QCOMPARE(secondPreview.version, 2);

    PreviewPackage approvedPreview;
    QVERIFY2(repository.reviewPreview(secondPreview.id, QStringLiteral("approved"), QStringLiteral("This preview is representative enough to move forward later."), &approvedPreview, &errorMessage), qPrintable(errorMessage));
    QCOMPARE(approvedPreview.approvalState, QStringLiteral("approved"));

    ProjectRepository reopened(tempDir.path());
    const QVector<Project> projects = reopened.all(&errorMessage);
    QVERIFY2(errorMessage.isEmpty(), qPrintable(errorMessage));
    QCOMPARE(projects.front().projectState, QStringLiteral("preview-ready"));

    const QVector<PreviewPackage> previews = reopened.listPreviews(created.id, &errorMessage);
    QVERIFY2(errorMessage.isEmpty(), qPrintable(errorMessage));
    QCOMPARE(previews.size(), 2);
    QCOMPARE(previews.front().version, 2);
    QCOMPARE(previews.front().approvalState, QStringLiteral("approved"));
    QCOMPARE(previews.back().version, 1);
    QCOMPARE(previews.back().approvalState, QStringLiteral("changes-requested"));
}

void ProjectRepositoryTest::full_build_requires_an_approved_preview()
{
    QTemporaryDir tempDir;
    QVERIFY2(tempDir.isValid(), "temporary directory should be available");

    ProjectRepository repository(tempDir.path());
    ProjectDraft draft {
        .gameTitle = QStringLiteral("Final Fantasy VI"),
        .platform = QStringLiteral("SNES"),
        .guideIntent = QStringLiteral("balanced route with trusted reference notes"),
        .playerGoal = QStringLiteral("finish the story and keep key missables safe"),
        .runStyle = QStringLiteral("casual"),
        .spoilerPolicy = QStringLiteral("minimal"),
        .depthPreference = QStringLiteral("standard"),
        .sharedThemeCss = QStringLiteral(".guide-theme-surface { --accent: #d6a55f; }"),
    };

    Project created;
    QString errorMessage;
    QVERIFY2(repository.create(draft, &created, &errorMessage), qPrintable(errorMessage));

    BlueprintDraft blueprintDraft {
        .chapterPlans = {
            BlueprintChapterPlan { QStringLiteral("Opening route"), QStringLiteral("Establish the early route, setup, and irreversible warnings.") },
            BlueprintChapterPlan { QStringLiteral("World of Balance core path"), QStringLiteral("Lay out the mid-game route and party checkpoints.") },
            BlueprintChapterPlan { QStringLiteral("World of Ruin cleanup"), QStringLiteral("Track optional reunions and final preparation.") },
        },
        .terminologyRules = QStringLiteral("Use official location, boss, and esper names."),
        .styleBible = QStringLiteral("Keep the tone calm, tactical, and book-like."),
        .crossReferencePlan = QStringLiteral("Tie route warnings back to reference notes and missable callouts."),
        .checklistPlan = QStringLiteral("End each chapter with a short irreversible-check list."),
        .visualPackIntent = QStringLiteral("Warm field-manual cards with compact tactical density."),
        .spoilerGuardrails = QStringLiteral("Keep plot reveals out of headings and route summaries."),
    };

    EditorialBlueprint blueprint;
    QVERIFY2(repository.createBlueprint(created.id, blueprintDraft, &blueprint, &errorMessage), qPrintable(errorMessage));

    PreviewPackage preview;
    QVERIFY2(repository.createPreview(created.id, &preview, &errorMessage), qPrintable(errorMessage));

    GuideBuildBundle bundle;
    QVERIFY2(!repository.createFullBuild(created.id, &bundle, &errorMessage), "full build should fail until the preview is approved");
    QVERIFY2(errorMessage.contains(QStringLiteral("preview"), Qt::CaseInsensitive), qPrintable(errorMessage));
}

void ProjectRepositoryTest::full_build_persists_bundle_and_freeze_pending_state()
{
    QTemporaryDir tempDir;
    QVERIFY2(tempDir.isValid(), "temporary directory should be available");

    ProjectRepository repository(tempDir.path());
    ProjectDraft draft {
        .gameTitle = QStringLiteral("Final Fantasy VI"),
        .platform = QStringLiteral("SNES"),
        .guideIntent = QStringLiteral("balanced route with trusted reference notes"),
        .playerGoal = QStringLiteral("finish the story and keep key missables safe"),
        .runStyle = QStringLiteral("casual"),
        .spoilerPolicy = QStringLiteral("minimal"),
        .depthPreference = QStringLiteral("standard"),
        .sharedThemeCss = QStringLiteral(".guide-theme-surface { --accent: #d6a55f; }"),
    };

    Project created;
    QString errorMessage;
    QVERIFY2(repository.create(draft, &created, &errorMessage), qPrintable(errorMessage));

    BlueprintDraft blueprintDraft {
        .chapterPlans = {
            BlueprintChapterPlan { QStringLiteral("Opening route"), QStringLiteral("Establish the early route, setup, and irreversible warnings.") },
            BlueprintChapterPlan { QStringLiteral("World of Balance core path"), QStringLiteral("Lay out the mid-game route and party checkpoints.") },
            BlueprintChapterPlan { QStringLiteral("World of Ruin cleanup"), QStringLiteral("Track optional reunions and final preparation.") },
        },
        .terminologyRules = QStringLiteral("Use official location, boss, and esper names."),
        .styleBible = QStringLiteral("Keep the tone calm, tactical, and book-like."),
        .crossReferencePlan = QStringLiteral("Tie route warnings back to reference notes and missable callouts."),
        .checklistPlan = QStringLiteral("End each chapter with a short irreversible-check list."),
        .visualPackIntent = QStringLiteral("Warm field-manual cards with compact tactical density."),
        .spoilerGuardrails = QStringLiteral("Keep plot reveals out of headings and route summaries."),
    };

    EditorialBlueprint blueprint;
    QVERIFY2(repository.createBlueprint(created.id, blueprintDraft, &blueprint, &errorMessage), qPrintable(errorMessage));

    KnowledgeEntityDraft visibleDraft {
        .canonicalName = QStringLiteral("Zozo clock clue"),
        .category = QStringLiteral("quest"),
        .visibility = QStringLiteral("visible-guide"),
        .scope = QStringLiteral("broad"),
        .confidence = QStringLiteral("high"),
        .aliases = { QStringLiteral("Clock puzzle") },
        .summary = QStringLiteral("The Zozo clock puzzle should be quoted with the solved time instead of paraphrasing the merchant clue chain."),
        .structuredAttributes = {
            KnowledgeAttribute { QStringLiteral("Correct answer"), QStringLiteral("6:10:50") },
            KnowledgeAttribute { QStringLiteral("Guide usage"), QStringLiteral("Visible route note and missable checkpoint callout") },
        },
        .sourceRefs = {
            SourceReference {
                .id = QString(),
                .sourceType = QStringLiteral("manual-entry"),
                .sourceTitle = QStringLiteral("SNES route notebook"),
                .sourceUri = QStringLiteral("https://example.com/ff6-route-notes"),
                .excerpt = QStringLiteral("Clock clue resolves to 6:10:50 once the merchant hints are combined correctly."),
                .retrievalDate = QStringLiteral("2026-04-22T00:00:00Z"),
                .trustClassification = QStringLiteral("community-verified"),
            },
        },
        .conflictMarkers = {},
        .versionTags = { QStringLiteral("snes") },
    };

    KnowledgeEntity visibleKnowledge;
    QVERIFY2(repository.createKnowledgeEntity(created.id, visibleDraft, &visibleKnowledge, &errorMessage), qPrintable(errorMessage));

    KnowledgeEntityDraft hiddenDraft {
        .canonicalName = QStringLiteral("Sketch glitch caution"),
        .category = QStringLiteral("system"),
        .visibility = QStringLiteral("hidden-codex"),
        .scope = QStringLiteral("run-specific"),
        .confidence = QStringLiteral("medium"),
        .aliases = { QStringLiteral("Sketch exploit") },
        .summary = QStringLiteral("Hidden codex note about unstable sketch behavior that should stay out of visible guide copy."),
        .structuredAttributes = {
            KnowledgeAttribute { QStringLiteral("Risk"), QStringLiteral("Potential save corruption on some versions") },
        },
        .sourceRefs = {
            SourceReference {
                .id = QString(),
                .sourceType = QStringLiteral("manual-entry"),
                .sourceTitle = QStringLiteral("Internal safety notes"),
                .sourceUri = QString(),
                .excerpt = QStringLiteral("Keep this in the hidden codex until a clean repro path is documented."),
                .retrievalDate = QStringLiteral("2026-04-22T00:00:00Z"),
                .trustClassification = QStringLiteral("working-notes"),
            },
        },
        .conflictMarkers = {},
        .versionTags = { QStringLiteral("snes") },
    };

    KnowledgeEntity hiddenKnowledge;
    QVERIFY2(repository.createKnowledgeEntity(created.id, hiddenDraft, &hiddenKnowledge, &errorMessage), qPrintable(errorMessage));

    PreviewPackage preview;
    QVERIFY2(repository.createPreview(created.id, &preview, &errorMessage), qPrintable(errorMessage));

    PreviewPackage approvedPreview;
    QVERIFY2(repository.reviewPreview(preview.id, QStringLiteral("approved"), QStringLiteral("Representative enough to continue into the first bounded full build."), &approvedPreview, &errorMessage), qPrintable(errorMessage));

    GuideBuildBundle buildBundle;
    QVERIFY2(repository.createFullBuild(created.id, &buildBundle, &errorMessage), qPrintable(errorMessage));
    QCOMPARE(buildBundle.version, 1);
    QCOMPARE(buildBundle.validationState, QStringLiteral("freeze-pending"));
    QVERIFY2(buildBundle.units.size() >= 5, "build bundle should contain bounded guide units");
    QCOMPARE(buildBundle.hiddenCodexIds.size(), 1);

    QString combinedBody;
    for (const GuideBuildUnit &unit : buildBundle.units) {
        combinedBody += unit.body;
        combinedBody += QStringLiteral("\n\n");
    }
    QVERIFY2(combinedBody.contains(QStringLiteral("Zozo clock clue")), "visible guide facts should appear in the build bundle");
    QVERIFY2(!combinedBody.contains(QStringLiteral("Sketch glitch caution")), "hidden codex notes should not leak into visible build text");

    const QVector<BuildJob> jobs = repository.listJobs(created.id, &errorMessage);
    QVERIFY2(errorMessage.isEmpty(), qPrintable(errorMessage));
    QCOMPARE(jobs.size(), 1);
    QCOMPARE(jobs.front().jobType, QStringLiteral("full-build"));
    QCOMPARE(jobs.front().status, QStringLiteral("requires-review"));

    const QVector<ValidationReport> reports = repository.listValidationReports(created.id, &errorMessage);
    QVERIFY2(errorMessage.isEmpty(), qPrintable(errorMessage));
    QCOMPARE(reports.size(), 1);
    QCOMPARE(reports.front().blockingCount, 0);

    ProjectRepository reopened(tempDir.path());
    const QVector<Project> projects = reopened.all(&errorMessage);
    QVERIFY2(errorMessage.isEmpty(), qPrintable(errorMessage));
    QCOMPARE(projects.size(), 1);
    QCOMPARE(projects.front().projectState, QStringLiteral("freeze-pending"));

    const QVector<GuideBuildBundle> bundles = reopened.listBuildBundles(created.id, &errorMessage);
    QVERIFY2(errorMessage.isEmpty(), qPrintable(errorMessage));
    QCOMPARE(bundles.size(), 1);
    QCOMPARE(bundles.front().validationState, QStringLiteral("freeze-pending"));
    QVERIFY2(QFile::exists(tempDir.path() + QStringLiteral("/projects/") + created.id + QStringLiteral("/build-bundles/") + buildBundle.id + QStringLiteral(".json")), "build bundle record should be written to the project workspace");
}

void ProjectRepositoryTest::full_build_with_blocking_visible_conflict_sets_validation_failed_state()
{
    QTemporaryDir tempDir;
    QVERIFY2(tempDir.isValid(), "temporary directory should be available");

    ProjectRepository repository(tempDir.path());
    ProjectDraft draft {
        .gameTitle = QStringLiteral("Final Fantasy VI"),
        .platform = QStringLiteral("SNES"),
        .guideIntent = QStringLiteral("balanced route with trusted reference notes"),
        .playerGoal = QStringLiteral("finish the story and keep key missables safe"),
        .runStyle = QStringLiteral("casual"),
        .spoilerPolicy = QStringLiteral("minimal"),
        .depthPreference = QStringLiteral("standard"),
        .sharedThemeCss = QStringLiteral(".guide-theme-surface { --accent: #d6a55f; }"),
    };

    Project created;
    QString errorMessage;
    QVERIFY2(repository.create(draft, &created, &errorMessage), qPrintable(errorMessage));

    BlueprintDraft blueprintDraft {
        .chapterPlans = {
            BlueprintChapterPlan { QStringLiteral("Opening route"), QStringLiteral("Establish the early route, setup, and irreversible warnings.") },
            BlueprintChapterPlan { QStringLiteral("World of Balance core path"), QStringLiteral("Lay out the mid-game route and party checkpoints.") },
            BlueprintChapterPlan { QStringLiteral("World of Ruin cleanup"), QStringLiteral("Track optional reunions and final preparation.") },
        },
        .terminologyRules = QStringLiteral("Use official location, boss, and esper names."),
        .styleBible = QStringLiteral("Keep the tone calm, tactical, and book-like."),
        .crossReferencePlan = QStringLiteral("Tie route warnings back to reference notes and missable callouts."),
        .checklistPlan = QStringLiteral("End each chapter with a short irreversible-check list."),
        .visualPackIntent = QStringLiteral("Warm field-manual cards with compact tactical density."),
        .spoilerGuardrails = QStringLiteral("Keep plot reveals out of headings and route summaries."),
    };

    EditorialBlueprint blueprint;
    QVERIFY2(repository.createBlueprint(created.id, blueprintDraft, &blueprint, &errorMessage), qPrintable(errorMessage));

    KnowledgeEntityDraft visibleDraft {
        .canonicalName = QStringLiteral("Zozo clock clue"),
        .category = QStringLiteral("quest"),
        .visibility = QStringLiteral("visible-guide"),
        .scope = QStringLiteral("broad"),
        .confidence = QStringLiteral("high"),
        .aliases = { QStringLiteral("Clock puzzle") },
        .summary = QStringLiteral("The Zozo clock puzzle should be quoted with the solved time instead of paraphrasing the merchant clue chain."),
        .structuredAttributes = {
            KnowledgeAttribute { QStringLiteral("Correct answer"), QStringLiteral("6:10:50") },
        },
        .sourceRefs = {
            SourceReference {
                .id = QString(),
                .sourceType = QStringLiteral("manual-entry"),
                .sourceTitle = QStringLiteral("SNES route notebook"),
                .sourceUri = QStringLiteral("https://example.com/ff6-route-notes"),
                .excerpt = QStringLiteral("Clock clue resolves to 6:10:50 once the merchant hints are combined correctly."),
                .retrievalDate = QStringLiteral("2026-04-22T00:00:00Z"),
                .trustClassification = QStringLiteral("community-verified"),
            },
        },
        .conflictMarkers = {
            ConflictRecord {
                .id = QString(),
                .conflictSummary = QStringLiteral("Two route notes disagree on whether the clock solution should be delayed until after the first detour."),
                .conflictType = QStringLiteral("route-order-disagreement"),
                .severity = QStringLiteral("blocking"),
                .resolutionStatus = QStringLiteral("open"),
                .recommendedHandling = QStringLiteral("Do not freeze the guide until the route order is re-verified against a clean save."),
            },
        },
        .versionTags = { QStringLiteral("snes") },
    };

    KnowledgeEntity visibleKnowledge;
    QVERIFY2(repository.createKnowledgeEntity(created.id, visibleDraft, &visibleKnowledge, &errorMessage), qPrintable(errorMessage));

    PreviewPackage preview;
    QVERIFY2(repository.createPreview(created.id, &preview, &errorMessage), qPrintable(errorMessage));

    PreviewPackage approvedPreview;
    QVERIFY2(repository.reviewPreview(preview.id, QStringLiteral("approved"), QStringLiteral("Representative enough to continue into the first bounded full build."), &approvedPreview, &errorMessage), qPrintable(errorMessage));

    GuideBuildBundle buildBundle;
    QVERIFY2(repository.createFullBuild(created.id, &buildBundle, &errorMessage), qPrintable(errorMessage));
    QCOMPARE(buildBundle.validationState, QStringLiteral("validation-failed"));

    const QVector<BuildJob> jobs = repository.listJobs(created.id, &errorMessage);
    QVERIFY2(errorMessage.isEmpty(), qPrintable(errorMessage));
    QCOMPARE(jobs.size(), 1);
    QCOMPARE(jobs.front().status, QStringLiteral("blocked"));

    const QVector<ValidationReport> reports = repository.listValidationReports(created.id, &errorMessage);
    QVERIFY2(errorMessage.isEmpty(), qPrintable(errorMessage));
    QCOMPARE(reports.size(), 1);
    QCOMPARE(reports.front().blockingCount, 1);

    ProjectRepository reopened(tempDir.path());
    const QVector<Project> projects = reopened.all(&errorMessage);
    QVERIFY2(errorMessage.isEmpty(), qPrintable(errorMessage));
    QCOMPARE(projects.size(), 1);
    QCOMPARE(projects.front().projectState, QStringLiteral("validation-failed"));
}

void ProjectRepositoryTest::knowledge_entities_require_provenance_and_persist_hidden_codex_entries()
{
    QTemporaryDir tempDir;
    QVERIFY2(tempDir.isValid(), "temporary directory should be available");

    ProjectRepository repository(tempDir.path());
    ProjectDraft draft {
        .gameTitle = QStringLiteral("Final Fantasy VI"),
        .platform = QStringLiteral("SNES"),
        .guideIntent = QStringLiteral("balanced route"),
        .playerGoal = QStringLiteral("finish the story"),
        .runStyle = QStringLiteral("casual"),
        .spoilerPolicy = QStringLiteral("minimal"),
        .depthPreference = QStringLiteral("standard"),
        .sharedThemeCss = QStringLiteral(),
    };

    Project created;
    QString errorMessage;
    QVERIFY2(repository.create(draft, &created, &errorMessage), qPrintable(errorMessage));

    KnowledgeEntityDraft invalidDraft {
        .canonicalName = QStringLiteral("Zozo clock clue"),
        .category = QStringLiteral("quest"),
        .visibility = QStringLiteral("visible-guide"),
        .scope = QStringLiteral("broad"),
        .confidence = QStringLiteral("medium"),
        .aliases = { QStringLiteral("Clock puzzle") },
        .summary = QStringLiteral("The Zozo clock puzzle unlocks the route forward once the clue text is decoded correctly."),
        .structuredAttributes = {
            KnowledgeAttribute { QStringLiteral("Correct answer"), QStringLiteral("6:10:50") },
        },
        .sourceRefs = {},
        .conflictMarkers = {},
        .versionTags = { QStringLiteral("snes") },
    };

    KnowledgeEntity knowledge;
    QVERIFY2(!repository.createKnowledgeEntity(created.id, invalidDraft, &knowledge, &errorMessage), "knowledge creation should fail without provenance");
    QVERIFY2(errorMessage.contains(QStringLiteral("source"), Qt::CaseInsensitive), qPrintable(errorMessage));

    KnowledgeEntityDraft visibleDraft {
        .canonicalName = QStringLiteral("Zozo clock clue"),
        .category = QStringLiteral("quest"),
        .visibility = QStringLiteral("visible-guide"),
        .scope = QStringLiteral("broad"),
        .confidence = QStringLiteral("medium"),
        .aliases = { QStringLiteral("Clock puzzle") },
        .summary = QStringLiteral("The Zozo clock puzzle unlocks the route forward once the clue text is decoded correctly."),
        .structuredAttributes = {
            KnowledgeAttribute { QStringLiteral("Correct answer"), QStringLiteral("6:10:50") },
            KnowledgeAttribute { QStringLiteral("Gate"), QStringLiteral("Required for Terra branch progress") },
        },
        .sourceRefs = {
            SourceReference {
                .id = QString(),
                .sourceType = QStringLiteral("manual-entry"),
                .sourceTitle = QStringLiteral("SNES playthrough notes"),
                .sourceUri = QStringLiteral("https://example.com/ff6-notes"),
                .excerpt = QStringLiteral("Clock clue leads to the correct setting once the merchant hints are combined."),
                .retrievalDate = QStringLiteral("2026-04-22T00:00:00Z"),
                .trustClassification = QStringLiteral("community-verified"),
            },
        },
        .conflictMarkers = {
            ConflictRecord {
                .id = QString(),
                .conflictSummary = QStringLiteral("Some fan notes swap the minute and second hands when paraphrasing the clue."),
                .conflictType = QStringLiteral("instruction-ambiguity"),
                .severity = QStringLiteral("warning"),
                .resolutionStatus = QStringLiteral("open"),
                .recommendedHandling = QStringLiteral("Quote the final solved time exactly instead of paraphrasing the hint chain."),
            },
        },
        .versionTags = { QStringLiteral("snes"), QStringLiteral("pixel-remaster") },
    };

    KnowledgeEntity visibleKnowledge;
    QVERIFY2(repository.createKnowledgeEntity(created.id, visibleDraft, &visibleKnowledge, &errorMessage), qPrintable(errorMessage));
    QCOMPARE(visibleKnowledge.visibility, QStringLiteral("visible-guide"));
    QCOMPARE(visibleKnowledge.sourceRefs.size(), 1);
    QCOMPARE(visibleKnowledge.conflictMarkers.size(), 1);

    KnowledgeEntityDraft hiddenDraft {
        .canonicalName = QStringLiteral("Sketch glitch caution"),
        .category = QStringLiteral("system"),
        .visibility = QStringLiteral("hidden-codex"),
        .scope = QStringLiteral("run-specific"),
        .confidence = QStringLiteral("low"),
        .aliases = { QStringLiteral("Sketch exploit") },
        .summary = QStringLiteral("Hidden codex note for unstable sketch behavior that should not be surfaced in the visible guide without extra verification."),
        .structuredAttributes = {
            KnowledgeAttribute { QStringLiteral("Risk"), QStringLiteral("Save corruption in some versions") },
        },
        .sourceRefs = {
            SourceReference {
                .id = QString(),
                .sourceType = QStringLiteral("manual-entry"),
                .sourceTitle = QStringLiteral("Safety notes"),
                .sourceUri = QString(),
                .excerpt = QStringLiteral("Hold this in the hidden codex until a cleaner repro path is documented."),
                .retrievalDate = QStringLiteral("2026-04-22T00:00:00Z"),
                .trustClassification = QStringLiteral("working-notes"),
            },
        },
        .conflictMarkers = {},
        .versionTags = { QStringLiteral("snes") },
    };

    KnowledgeEntity hiddenKnowledge;
    QVERIFY2(repository.createKnowledgeEntity(created.id, hiddenDraft, &hiddenKnowledge, &errorMessage), qPrintable(errorMessage));
    QCOMPARE(hiddenKnowledge.visibility, QStringLiteral("hidden-codex"));

    const QVector<Project> projects = repository.all(&errorMessage);
    QVERIFY2(errorMessage.isEmpty(), qPrintable(errorMessage));
    QCOMPARE(projects.size(), 1);
    QCOMPARE(projects.front().projectState, QStringLiteral("knowledge-building"));

    const QVector<KnowledgeEntity> entities = repository.listKnowledgeEntities(created.id, &errorMessage);
    QVERIFY2(errorMessage.isEmpty(), qPrintable(errorMessage));
    QCOMPARE(entities.size(), 2);
    QCOMPARE(entities.front().visibility, QStringLiteral("hidden-codex"));
    QCOMPARE(entities.back().visibility, QStringLiteral("visible-guide"));
    QVERIFY2(QFile::exists(tempDir.path() + QStringLiteral("/projects/") + created.id + QStringLiteral("/knowledge/") + visibleKnowledge.id + QStringLiteral(".json")), "visible knowledge record should be written to the knowledge workspace");
}

void ProjectRepositoryTest::knowledge_entity_updates_survive_reopen_without_regressing_preview_state()
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

    BlueprintDraft blueprintDraft {
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

    EditorialBlueprint blueprint;
    QVERIFY2(repository.createBlueprint(created.id, blueprintDraft, &blueprint, &errorMessage), qPrintable(errorMessage));

    PreviewPackage preview;
    QVERIFY2(repository.createPreview(created.id, &preview, &errorMessage), qPrintable(errorMessage));

    KnowledgeEntityDraft knowledgeDraft {
        .canonicalName = QStringLiteral("Dragon reach breakpoint"),
        .category = QStringLiteral("system"),
        .visibility = QStringLiteral("hidden-codex"),
        .scope = QStringLiteral("run-specific"),
        .confidence = QStringLiteral("medium"),
        .aliases = { QStringLiteral("Break arts threshold") },
        .summary = QStringLiteral("Hidden codex note tracking when dragon-affinity reach starts to outscale generic safety setups."),
        .structuredAttributes = {
            KnowledgeAttribute { QStringLiteral("Risk profile"), QStringLiteral("High if gear affinity lags behind") },
        },
        .sourceRefs = {
            SourceReference {
                .id = QString(),
                .sourceType = QStringLiteral("manual-entry"),
                .sourceTitle = QStringLiteral("Workshop test file"),
                .sourceUri = QString(),
                .excerpt = QStringLiteral("Damage stability changes once dragon reach and affinity prep line up."),
                .retrievalDate = QStringLiteral("2026-04-22T00:00:00Z"),
                .trustClassification = QStringLiteral("working-notes"),
            },
        },
        .conflictMarkers = {},
        .versionTags = { QStringLiteral("ps1") },
    };

    KnowledgeEntity knowledge;
    QVERIFY2(repository.createKnowledgeEntity(created.id, knowledgeDraft, &knowledge, &errorMessage), qPrintable(errorMessage));

    KnowledgeEntityDraft updatedDraft {
        .canonicalName = QStringLiteral("Dragon reach breakpoint"),
        .category = QStringLiteral("system"),
        .visibility = QStringLiteral("hidden-codex"),
        .scope = QStringLiteral("run-specific"),
        .confidence = QStringLiteral("high"),
        .aliases = { QStringLiteral("Break arts threshold"), QStringLiteral("Dragon affinity breakpoint") },
        .summary = QStringLiteral("Hidden codex note confirming the safer breakpoint for dragon reach scaling after affinity prep is locked in."),
        .structuredAttributes = {
            KnowledgeAttribute { QStringLiteral("Risk profile"), QStringLiteral("Stable once affinity prep is complete") },
            KnowledgeAttribute { QStringLiteral("Guide usage"), QStringLiteral("Reference only; do not surface in spoiler-light chapter copy") },
        },
        .sourceRefs = {
            SourceReference {
                .id = QString(),
                .sourceType = QStringLiteral("manual-entry"),
                .sourceTitle = QStringLiteral("Workshop test file"),
                .sourceUri = QString(),
                .excerpt = QStringLiteral("Confirmed breakpoint after repeated affinity setup runs."),
                .retrievalDate = QStringLiteral("2026-04-22T00:00:00Z"),
                .trustClassification = QStringLiteral("working-notes"),
            },
        },
        .conflictMarkers = {
            ConflictRecord {
                .id = QString(),
                .conflictSummary = QStringLiteral("Older community notes cite the breakpoint one chapter earlier."),
                .conflictType = QStringLiteral("timing-disagreement"),
                .severity = QStringLiteral("warning"),
                .resolutionStatus = QStringLiteral("monitoring"),
                .recommendedHandling = QStringLiteral("Keep this codex-only until the route note is validated against another clean save."),
            },
        },
        .versionTags = { QStringLiteral("ps1"), QStringLiteral("route-b") },
    };

    KnowledgeEntity updatedKnowledge;
    QVERIFY2(repository.updateKnowledgeEntity(knowledge.id, updatedDraft, &updatedKnowledge, &errorMessage), qPrintable(errorMessage));
    QCOMPARE(updatedKnowledge.confidence, QStringLiteral("high"));
    QCOMPARE(updatedKnowledge.conflictMarkers.size(), 1);

    ProjectRepository reopened(tempDir.path());
    const QVector<Project> projects = reopened.all(&errorMessage);
    QVERIFY2(errorMessage.isEmpty(), qPrintable(errorMessage));
    QCOMPARE(projects.size(), 1);
    QCOMPARE(projects.front().projectState, QStringLiteral("preview-ready"));

    const QVector<KnowledgeEntity> entities = reopened.listKnowledgeEntities(created.id, &errorMessage);
    QVERIFY2(errorMessage.isEmpty(), qPrintable(errorMessage));
    QCOMPARE(entities.size(), 1);
    QCOMPARE(entities.front().visibility, QStringLiteral("hidden-codex"));
    QCOMPARE(entities.front().confidence, QStringLiteral("high"));
    QVERIFY2(entities.front().aliases.contains(QStringLiteral("Dragon affinity breakpoint")), "updated aliases should be persisted");
}

QTEST_MAIN(ProjectRepositoryTest)
#include "project_repository_test.moc"

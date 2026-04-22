#include "project_repository.hpp"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QSaveFile>
#include <QStandardPaths>

#include <algorithm>
#include <utility>

namespace {
void setError(QString *errorMessage, const QString &message)
{
    if (errorMessage != nullptr) {
        *errorMessage = message;
    }
}

bool validateDraft(const ProjectDraft &draft, Project *projectOutput, QString *errorMessage)
{
    if (draft.gameTitle.trimmed().isEmpty()) {
        setError(errorMessage, QStringLiteral("Game title is required."));
        return false;
    }
    if (draft.guideIntent.trimmed().isEmpty()) {
        setError(errorMessage, QStringLiteral("Guide intent is required."));
        return false;
    }
    if (projectOutput == nullptr) {
        setError(errorMessage, QStringLiteral("Internal error: missing project output."));
        return false;
    }

    return true;
}

bool validateBlueprintDraft(const BlueprintDraft &draft, EditorialBlueprint *blueprintOutput, QString *errorMessage)
{
    if (draft.chapterPlans.isEmpty()) {
        setError(errorMessage, QStringLiteral("At least one chapter plan is required."));
        return false;
    }

    for (int index = 0; index < draft.chapterPlans.size(); ++index) {
        const BlueprintChapterPlan &chapterPlan = draft.chapterPlans.at(index);
        if (chapterPlan.title.trimmed().isEmpty() || chapterPlan.purpose.trimmed().isEmpty()) {
            setError(errorMessage, QStringLiteral("Chapter plan %1 needs both a title and a purpose.").arg(index + 1));
            return false;
        }
    }

    if (draft.terminologyRules.trimmed().isEmpty()) {
        setError(errorMessage, QStringLiteral("Terminology rules are required."));
        return false;
    }
    if (draft.styleBible.trimmed().isEmpty()) {
        setError(errorMessage, QStringLiteral("Style bible is required."));
        return false;
    }
    if (draft.crossReferencePlan.trimmed().isEmpty()) {
        setError(errorMessage, QStringLiteral("Cross-reference plan is required."));
        return false;
    }
    if (draft.checklistPlan.trimmed().isEmpty()) {
        setError(errorMessage, QStringLiteral("Checklist plan is required."));
        return false;
    }
    if (draft.visualPackIntent.trimmed().isEmpty()) {
        setError(errorMessage, QStringLiteral("Visual pack intent is required."));
        return false;
    }
    if (draft.spoilerGuardrails.trimmed().isEmpty()) {
        setError(errorMessage, QStringLiteral("Spoiler guardrails are required."));
        return false;
    }
    if (blueprintOutput == nullptr) {
        setError(errorMessage, QStringLiteral("Internal error: missing blueprint output."));
        return false;
    }

    return true;
}

void applyDraft(Project &project, const ProjectDraft &draft)
{
    project.gameTitle = draft.gameTitle.trimmed();
    project.platform = draft.platform.trimmed();
    project.guideIntent = draft.guideIntent.trimmed();
    project.playerGoal = draft.playerGoal.trimmed();
    project.runStyle = draft.runStyle.trimmed();
    project.spoilerPolicy = draft.spoilerPolicy.trimmed();
    project.depthPreference = draft.depthPreference.trimmed();
    project.sharedThemeCss = draft.sharedThemeCss;
}

QStringList artifactDirectoryNames()
{
    return {
        QStringLiteral("editions"),
        QStringLiteral("blueprints"),
        QStringLiteral("previews"),
        QStringLiteral("assets"),
        QStringLiteral("jobs"),
        QStringLiteral("validations"),
        QStringLiteral("exports"),
    };
}

QString currentIsoTimestamp()
{
    return QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
}

QString summarizeValidation(const ValidationReport &report)
{
    if (report.blockingCount == 0 && report.warningCount == 0) {
        return QStringLiteral("Intake check passed with no blockers or warnings.");
    }

    return QStringLiteral("%1 blocker(s), %2 warning(s)")
        .arg(report.blockingCount)
        .arg(report.warningCount);
}

ValidationReport buildIntakeValidationReport(const Project &project)
{
    ValidationReport report;
    report.id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    report.projectId = project.id;
    report.targetType = QStringLiteral("project-manifest");
    report.targetId = project.id;
    report.checksRun = {
        QStringLiteral("required-intake-fields"),
        QStringLiteral("platform-accuracy-signal"),
        QStringLiteral("intake-state-readiness"),
    };

    if (project.platform.trimmed().isEmpty()) {
        report.blockingFindings.push_back(QStringLiteral("Platform or version constraints are unresolved, so accuracy-sensitive guide work should not advance yet."));
    }

    if (project.playerGoal.trimmed().isEmpty()) {
        report.warningFindings.push_back(QStringLiteral("Player goal is unresolved, so the guide intent is still broad and may need refinement."));
    }

    if (project.sharedThemeCss.trimmed().isEmpty()) {
        report.warningFindings.push_back(QStringLiteral("No shared theme CSS is stored yet, so the web app and future guide surfaces will keep the default editorial theme."));
    }

    report.blockingCount = report.blockingFindings.size();
    report.warningCount = report.warningFindings.size();
    report.createdAt = currentIsoTimestamp();
    report.summary = summarizeValidation(report);
    return report;
}

bool validatePreviewApprovalState(const QString &approvalState, const QString &reviewNotes, QString *errorMessage)
{
    static const QStringList allowedStates {
        QStringLiteral("requires-review"),
        QStringLiteral("approved"),
        QStringLiteral("changes-requested"),
    };

    if (!allowedStates.contains(approvalState.trimmed())) {
        setError(errorMessage, QStringLiteral("Preview approval state must be requires-review, approved, or changes-requested."));
        return false;
    }

    if (approvalState.trimmed() == QStringLiteral("changes-requested") && reviewNotes.trimmed().isEmpty()) {
        setError(errorMessage, QStringLiteral("Review notes are required when changes are requested."));
        return false;
    }

    return true;
}

PreviewSampleSection buildFrontMatterPreviewSection(const Project &project, const EditorialBlueprint &blueprint)
{
    const QString platform = project.platform.trimmed().isEmpty() ? QStringLiteral("the chosen platform") : project.platform.trimmed();
    const QString playerGoal = project.playerGoal.trimmed().isEmpty() ? QStringLiteral("the chosen run goal") : project.playerGoal.trimmed();

    return PreviewSampleSection {
        .title = QStringLiteral("Preview frame"),
        .kind = QStringLiteral("front-matter"),
        .summary = QStringLiteral("Tone, scope, and spoiler contract for the review slice."),
        .body = QStringLiteral("%1 — %2\n\nPlatform focus: %3\nPlayer goal: %4\nRun style: %5\nDepth target: %6\n\nTone contract: %7\nSpoiler guardrails: %8")
            .arg(project.displayTitle(),
                 project.guideIntent,
                 platform,
                 playerGoal,
                 project.runStyle,
                 project.depthPreference,
                 blueprint.styleBible,
                 blueprint.spoilerGuardrails),
    };
}

PreviewSampleSection buildWalkthroughPreviewSection(const Project &project, const EditorialBlueprint &blueprint)
{
    const BlueprintChapterPlan primaryChapter = blueprint.chapterPlans.front();
    const BlueprintChapterPlan supportingChapter = blueprint.chapterPlans.size() > 1 ? blueprint.chapterPlans.at(1) : primaryChapter;
    const QString playerGoal = project.playerGoal.trimmed().isEmpty() ? QStringLiteral("the chosen run goal") : project.playerGoal.trimmed();

    return PreviewSampleSection {
        .title = primaryChapter.title,
        .kind = QStringLiteral("walkthrough"),
        .summary = primaryChapter.purpose,
        .body = QStringLiteral("Objective: %1\nRoute stance: Guide the player toward %2 with a %3 rhythm.\nSupporting chapter: %4 — %5\n\nCross-reference behavior: %6\nTerminology discipline: %7")
            .arg(primaryChapter.purpose,
                 playerGoal,
                 project.runStyle,
                 supportingChapter.title,
                 supportingChapter.purpose,
                 blueprint.crossReferencePlan,
                 blueprint.terminologyRules),
    };
}

PreviewSampleSection buildChecklistPreviewSection(const Project &project, const EditorialBlueprint &blueprint)
{
    const BlueprintChapterPlan firstChapter = blueprint.chapterPlans.front();
    const BlueprintChapterPlan secondChapter = blueprint.chapterPlans.size() > 1 ? blueprint.chapterPlans.at(1) : firstChapter;
    const QString playerGoal = project.playerGoal.trimmed().isEmpty() ? QStringLiteral("the chosen run goal") : project.playerGoal.trimmed();

    return PreviewSampleSection {
        .title = QStringLiteral("Checklist cadence sample"),
        .kind = QStringLiteral("checklist"),
        .summary = QStringLiteral("How route chapters end with explicit checks before progress continues."),
        .body = QStringLiteral("Before leaving %1:\n- Confirm the route still serves %2.\n- Capture irreversible warnings in a short tactical list.\n- Call out optional detours that should link forward to %3.\n\nChecklist rule: %4")
            .arg(firstChapter.title,
                 playerGoal,
                 secondChapter.title,
                 blueprint.checklistPlan),
    };
}

PreviewSampleSection buildReferencePreviewSection(const Project &project, const EditorialBlueprint &blueprint)
{
    return PreviewSampleSection {
        .title = QStringLiteral("Reference formatting sample"),
        .kind = QStringLiteral("reference"),
        .summary = QStringLiteral("How terminology, visual notes, and cross-references should read in the guide."),
        .body = QStringLiteral("Reference note for %1\nPreferred terminology: %2\nVisual direction: %3\nReference linkage: %4\nSpoiler policy in effect: %5")
            .arg(project.displayTitle(),
                 blueprint.terminologyRules,
                 blueprint.visualPackIntent,
                 blueprint.crossReferencePlan,
                 project.spoilerPolicy),
    };
}

PreviewPackage buildPreviewPackage(const Project &project, const EditorialBlueprint &blueprint, int version)
{
    PreviewPackage preview;
    preview.id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    preview.projectId = project.id;
    preview.blueprintVersion = blueprint.version;
    preview.version = version;
    preview.sampleSections = {
        buildFrontMatterPreviewSection(project, blueprint),
        buildWalkthroughPreviewSection(project, blueprint),
        buildChecklistPreviewSection(project, blueprint),
        buildReferencePreviewSection(project, blueprint),
    };
    preview.reviewNotes.clear();
    preview.approvalState = QStringLiteral("requires-review");
    preview.generatedAt = currentIsoTimestamp();
    preview.updatedAt = preview.generatedAt;
    return preview;
}
}

ProjectRepository::ProjectRepository(QString storageRoot)
    : m_storageRoot(std::move(storageRoot))
{
}

QString ProjectRepository::resolvedStorageRoot() const
{
    if (!m_storageRoot.trimmed().isEmpty()) {
        return m_storageRoot;
    }

    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (appDataPath.trimmed().isEmpty()) {
        appDataPath = QDir::homePath() + QStringLiteral("/.my-plaithrough");
    }
    return appDataPath;
}

QString ProjectRepository::storageRootPath() const
{
    return resolvedStorageRoot();
}

QString ProjectRepository::projectsRootPath() const
{
    return storageRootPath() + QStringLiteral("/projects");
}

QString ProjectRepository::projectRootPath(const QString &projectId) const
{
    return projectsRootPath() + QStringLiteral("/") + projectId;
}

QString ProjectRepository::legacyProjectsFilePath() const
{
    return storageRootPath() + QStringLiteral("/projects.json");
}

QString ProjectRepository::legacyBackupFilePath() const
{
    return storageRootPath() + QStringLiteral("/projects.legacy-imported.json");
}

QString ProjectRepository::projectManifestPath(const QString &projectId) const
{
    return projectRootPath(projectId) + QStringLiteral("/project-manifest.json");
}

QString ProjectRepository::blueprintsRootPath(const QString &projectId) const
{
    return projectRootPath(projectId) + QStringLiteral("/blueprints");
}

QString ProjectRepository::blueprintPath(const QString &projectId, const QString &blueprintId) const
{
    return blueprintsRootPath(projectId) + QStringLiteral("/") + blueprintId + QStringLiteral(".json");
}

QString ProjectRepository::previewsRootPath(const QString &projectId) const
{
    return projectRootPath(projectId) + QStringLiteral("/previews");
}

QString ProjectRepository::previewPath(const QString &projectId, const QString &previewId) const
{
    return previewsRootPath(projectId) + QStringLiteral("/") + previewId + QStringLiteral(".json");
}

QString ProjectRepository::jobsRootPath(const QString &projectId) const
{
    return projectRootPath(projectId) + QStringLiteral("/jobs");
}

QString ProjectRepository::jobPath(const QString &projectId, const QString &jobId) const
{
    return jobsRootPath(projectId) + QStringLiteral("/") + jobId + QStringLiteral(".json");
}

QString ProjectRepository::validationsRootPath(const QString &projectId) const
{
    return projectRootPath(projectId) + QStringLiteral("/validations");
}

QString ProjectRepository::validationReportPath(const QString &projectId, const QString &reportId) const
{
    return validationsRootPath(projectId) + QStringLiteral("/") + reportId + QStringLiteral(".json");
}

bool ProjectRepository::ensureProjectsRootExists(QString *errorMessage) const
{
    setError(errorMessage, {});

    QDir rootDirectory(storageRootPath());
    if (!rootDirectory.exists() && !rootDirectory.mkpath(QStringLiteral("."))) {
        setError(errorMessage, QStringLiteral("Could not create storage directory %1.").arg(rootDirectory.path()));
        return false;
    }

    QDir projectsDirectory(projectsRootPath());
    if (!projectsDirectory.exists() && !projectsDirectory.mkpath(QStringLiteral("."))) {
        setError(errorMessage, QStringLiteral("Could not create projects directory %1.").arg(projectsDirectory.path()));
        return false;
    }

    return true;
}

bool ProjectRepository::ensureProjectLayout(const QString &projectId, QString *errorMessage) const
{
    if (!ensureProjectsRootExists(errorMessage)) {
        return false;
    }

    QDir projectDirectory(projectRootPath(projectId));
    if (!projectDirectory.exists() && !projectDirectory.mkpath(QStringLiteral("."))) {
        setError(errorMessage, QStringLiteral("Could not create project workspace %1.").arg(projectDirectory.path()));
        return false;
    }

    for (const QString &directoryName : artifactDirectoryNames()) {
        if (!projectDirectory.exists(directoryName) && !projectDirectory.mkpath(directoryName)) {
            setError(errorMessage, QStringLiteral("Could not create artifact directory %1/%2.").arg(projectDirectory.path(), directoryName));
            return false;
        }
    }

    return true;
}

bool ProjectRepository::writeProject(const Project &project, QString *errorMessage) const
{
    setError(errorMessage, {});

    if (!ensureProjectLayout(project.id, errorMessage)) {
        return false;
    }

    QSaveFile file(projectManifestPath(project.id));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        setError(errorMessage, QStringLiteral("Could not open %1 for writing.").arg(projectManifestPath(project.id)));
        return false;
    }

    const QJsonDocument document(project.toJson());
    file.write(document.toJson(QJsonDocument::Indented));

    if (!file.commit()) {
        setError(errorMessage, QStringLiteral("Could not commit project data to %1.").arg(projectManifestPath(project.id)));
        return false;
    }

    return true;
}

bool ProjectRepository::writeBlueprint(const EditorialBlueprint &blueprint, QString *errorMessage) const
{
    setError(errorMessage, {});

    if (!ensureProjectLayout(blueprint.projectId, errorMessage)) {
        return false;
    }

    QSaveFile file(blueprintPath(blueprint.projectId, blueprint.id));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        setError(errorMessage, QStringLiteral("Could not open %1 for writing.").arg(blueprintPath(blueprint.projectId, blueprint.id)));
        return false;
    }

    file.write(QJsonDocument(blueprint.toJson()).toJson(QJsonDocument::Indented));
    if (!file.commit()) {
        setError(errorMessage, QStringLiteral("Could not commit blueprint record to %1.").arg(blueprintPath(blueprint.projectId, blueprint.id)));
        return false;
    }

    return true;
}

bool ProjectRepository::writePreview(const PreviewPackage &preview, QString *errorMessage) const
{
    setError(errorMessage, {});

    if (!ensureProjectLayout(preview.projectId, errorMessage)) {
        return false;
    }

    QSaveFile file(previewPath(preview.projectId, preview.id));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        setError(errorMessage, QStringLiteral("Could not open %1 for writing.").arg(previewPath(preview.projectId, preview.id)));
        return false;
    }

    file.write(QJsonDocument(preview.toJson()).toJson(QJsonDocument::Indented));
    if (!file.commit()) {
        setError(errorMessage, QStringLiteral("Could not commit preview record to %1.").arg(previewPath(preview.projectId, preview.id)));
        return false;
    }

    return true;
}

bool ProjectRepository::writeJob(const BuildJob &job, QString *errorMessage) const
{
    setError(errorMessage, {});

    if (!ensureProjectLayout(job.projectId, errorMessage)) {
        return false;
    }

    QSaveFile file(jobPath(job.projectId, job.id));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        setError(errorMessage, QStringLiteral("Could not open %1 for writing.").arg(jobPath(job.projectId, job.id)));
        return false;
    }

    file.write(QJsonDocument(job.toJson()).toJson(QJsonDocument::Indented));
    if (!file.commit()) {
        setError(errorMessage, QStringLiteral("Could not commit job record to %1.").arg(jobPath(job.projectId, job.id)));
        return false;
    }

    return true;
}

bool ProjectRepository::writeValidationReport(const ValidationReport &report, QString *errorMessage) const
{
    setError(errorMessage, {});

    if (!ensureProjectLayout(report.projectId, errorMessage)) {
        return false;
    }

    QSaveFile file(validationReportPath(report.projectId, report.id));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        setError(errorMessage, QStringLiteral("Could not open %1 for writing.").arg(validationReportPath(report.projectId, report.id)));
        return false;
    }

    file.write(QJsonDocument(report.toJson()).toJson(QJsonDocument::Indented));
    if (!file.commit()) {
        setError(errorMessage, QStringLiteral("Could not commit validation report to %1.").arg(validationReportPath(report.projectId, report.id)));
        return false;
    }

    return true;
}

bool ProjectRepository::loadProjectFromFile(const QString &filePath, Project *project, QString *errorMessage) const
{
    setError(errorMessage, {});

    if (project == nullptr) {
        setError(errorMessage, QStringLiteral("Internal error: missing project output."));
        return false;
    }

    QFile file(filePath);
    if (!file.exists()) {
        setError(errorMessage, QStringLiteral("Project manifest %1 does not exist.").arg(filePath));
        return false;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        setError(errorMessage, QStringLiteral("Could not open %1 for reading.").arg(filePath));
        return false;
    }

    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        setError(errorMessage, QStringLiteral("Could not parse %1: %2").arg(filePath, parseError.errorString()));
        return false;
    }

    if (!document.isObject()) {
        setError(errorMessage, QStringLiteral("Project manifest %1 is corrupted: expected a JSON object.").arg(filePath));
        return false;
    }

    const Project loadedProject = Project::fromJson(document.object());
    if (loadedProject.id.trimmed().isEmpty() || loadedProject.gameTitle.trimmed().isEmpty() || loadedProject.guideIntent.trimmed().isEmpty()) {
        setError(errorMessage, QStringLiteral("Project manifest %1 is corrupted: required fields are missing.").arg(filePath));
        return false;
    }

    *project = loadedProject;
    return true;
}

bool ProjectRepository::loadProject(const QString &projectId, Project *project, QString *errorMessage) const
{
    return loadProjectFromFile(projectManifestPath(projectId), project, errorMessage);
}

bool ProjectRepository::loadBlueprintFromFile(const QString &filePath, EditorialBlueprint *blueprint, QString *errorMessage) const
{
    setError(errorMessage, {});

    if (blueprint == nullptr) {
        setError(errorMessage, QStringLiteral("Internal error: missing blueprint output."));
        return false;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        setError(errorMessage, QStringLiteral("Could not open %1 for reading.").arg(filePath));
        return false;
    }

    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        setError(errorMessage, QStringLiteral("Could not parse blueprint record %1.").arg(filePath));
        return false;
    }

    const EditorialBlueprint loadedBlueprint = EditorialBlueprint::fromJson(document.object());
    if (loadedBlueprint.id.trimmed().isEmpty() || loadedBlueprint.projectId.trimmed().isEmpty() || loadedBlueprint.chapterPlans.isEmpty()) {
        setError(errorMessage, QStringLiteral("Blueprint record %1 is corrupted: required fields are missing.").arg(filePath));
        return false;
    }

    *blueprint = loadedBlueprint;
    return true;
}

bool ProjectRepository::loadPreviewFromFile(const QString &filePath, PreviewPackage *preview, QString *errorMessage) const
{
    setError(errorMessage, {});

    if (preview == nullptr) {
        setError(errorMessage, QStringLiteral("Internal error: missing preview output."));
        return false;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        setError(errorMessage, QStringLiteral("Could not open %1 for reading.").arg(filePath));
        return false;
    }

    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        setError(errorMessage, QStringLiteral("Could not parse preview record %1.").arg(filePath));
        return false;
    }

    const PreviewPackage loadedPreview = PreviewPackage::fromJson(document.object());
    if (loadedPreview.id.trimmed().isEmpty() || loadedPreview.projectId.trimmed().isEmpty() || loadedPreview.sampleSections.isEmpty()) {
        setError(errorMessage, QStringLiteral("Preview record %1 is corrupted: required fields are missing.").arg(filePath));
        return false;
    }

    *preview = loadedPreview;
    return true;
}

bool ProjectRepository::loadJobFromFile(const QString &filePath, BuildJob *job, QString *errorMessage) const
{
    setError(errorMessage, {});

    if (job == nullptr) {
        setError(errorMessage, QStringLiteral("Internal error: missing job output."));
        return false;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        setError(errorMessage, QStringLiteral("Could not open %1 for reading.").arg(filePath));
        return false;
    }

    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        setError(errorMessage, QStringLiteral("Could not parse job record %1.").arg(filePath));
        return false;
    }

    const BuildJob loadedJob = BuildJob::fromJson(document.object());
    if (loadedJob.id.trimmed().isEmpty() || loadedJob.projectId.trimmed().isEmpty() || loadedJob.jobType.trimmed().isEmpty()) {
        setError(errorMessage, QStringLiteral("Job record %1 is corrupted: required fields are missing.").arg(filePath));
        return false;
    }

    *job = loadedJob;
    return true;
}

bool ProjectRepository::loadValidationReportFromFile(const QString &filePath, ValidationReport *report, QString *errorMessage) const
{
    setError(errorMessage, {});

    if (report == nullptr) {
        setError(errorMessage, QStringLiteral("Internal error: missing validation report output."));
        return false;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        setError(errorMessage, QStringLiteral("Could not open %1 for reading.").arg(filePath));
        return false;
    }

    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        setError(errorMessage, QStringLiteral("Could not parse validation report %1.").arg(filePath));
        return false;
    }

    const ValidationReport loadedReport = ValidationReport::fromJson(document.object());
    if (loadedReport.id.trimmed().isEmpty() || loadedReport.projectId.trimmed().isEmpty()) {
        setError(errorMessage, QStringLiteral("Validation report %1 is corrupted: required fields are missing.").arg(filePath));
        return false;
    }

    *report = loadedReport;
    return true;
}

bool ProjectRepository::migrateLegacyStoreIfNeeded(QString *errorMessage) const
{
    setError(errorMessage, {});

    const QString legacyFilePath = legacyProjectsFilePath();
    QFile legacyFile(legacyFilePath);
    if (!legacyFile.exists()) {
        return true;
    }

    if (!ensureProjectsRootExists(errorMessage)) {
        return false;
    }

    if (!legacyFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        setError(errorMessage, QStringLiteral("Could not open %1 for reading.").arg(legacyFilePath));
        return false;
    }

    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(legacyFile.readAll(), &parseError);
    legacyFile.close();

    if (parseError.error != QJsonParseError::NoError) {
        setError(errorMessage, QStringLiteral("Could not parse %1: %2").arg(legacyFilePath, parseError.errorString()));
        return false;
    }

    if (!document.isArray()) {
        setError(errorMessage, QStringLiteral("Legacy project store is corrupted: expected a JSON array."));
        return false;
    }

    const QJsonArray array = document.array();
    for (const QJsonValue &value : array) {
        if (!value.isObject()) {
            setError(errorMessage, QStringLiteral("Legacy project store is corrupted: array item was not an object."));
            return false;
        }

        Project project = Project::fromJson(value.toObject());
        if (project.id.trimmed().isEmpty() || project.gameTitle.trimmed().isEmpty() || project.guideIntent.trimmed().isEmpty()) {
            setError(errorMessage, QStringLiteral("Legacy project store is corrupted: required project fields are missing."));
            return false;
        }
        if (project.projectState.trimmed().isEmpty()) {
            project.projectState = QStringLiteral("intake");
        }

        const QString manifestPath = projectManifestPath(project.id);
        if (!QFileInfo::exists(manifestPath) && !writeProject(project, errorMessage)) {
            return false;
        }
    }

    QFile backupFile(legacyBackupFilePath());
    if (backupFile.exists() && !backupFile.remove()) {
        setError(errorMessage, QStringLiteral("Could not replace the old legacy backup %1.").arg(backupFile.fileName()));
        return false;
    }

    if (!legacyFile.rename(legacyBackupFilePath())) {
        setError(errorMessage, QStringLiteral("Could not move legacy project store to %1.").arg(legacyBackupFilePath()));
        return false;
    }

    return true;
}

QVector<Project> ProjectRepository::all(QString *errorMessage) const
{
    setError(errorMessage, {});

    if (!migrateLegacyStoreIfNeeded(errorMessage)) {
        return {};
    }

    QDir projectsDirectory(projectsRootPath());
    if (!projectsDirectory.exists()) {
        return {};
    }

    QVector<Project> projects;
    const QStringList projectIds = projectsDirectory.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
    projects.reserve(projectIds.size());

    for (const QString &projectId : projectIds) {
        Project project;
        if (!loadProject(projectId, &project, errorMessage)) {
            return {};
        }
        projects.push_back(project);
    }

    std::sort(projects.begin(), projects.end(), [](const Project &lhs, const Project &rhs) {
        return lhs.createdAt > rhs.createdAt;
    });

    return projects;
}

QVector<EditorialBlueprint> ProjectRepository::listBlueprints(const QString &projectId, QString *errorMessage) const
{
    setError(errorMessage, {});

    if (!migrateLegacyStoreIfNeeded(errorMessage)) {
        return {};
    }

    QDir blueprintsDirectory(blueprintsRootPath(projectId));
    if (!blueprintsDirectory.exists()) {
        return {};
    }

    QVector<EditorialBlueprint> blueprints;
    const QStringList files = blueprintsDirectory.entryList({QStringLiteral("*.json")}, QDir::Files, QDir::Name);
    blueprints.reserve(files.size());

    for (const QString &fileName : files) {
        EditorialBlueprint blueprint;
        if (!loadBlueprintFromFile(blueprintsDirectory.filePath(fileName), &blueprint, errorMessage)) {
            return {};
        }
        blueprints.push_back(blueprint);
    }

    std::sort(blueprints.begin(), blueprints.end(), [](const EditorialBlueprint &lhs, const EditorialBlueprint &rhs) {
        if (lhs.version != rhs.version) {
            return lhs.version > rhs.version;
        }

        return lhs.updatedAt > rhs.updatedAt;
    });

    return blueprints;
}

QVector<PreviewPackage> ProjectRepository::listPreviews(const QString &projectId, QString *errorMessage) const
{
    setError(errorMessage, {});

    if (!migrateLegacyStoreIfNeeded(errorMessage)) {
        return {};
    }

    QDir previewsDirectory(previewsRootPath(projectId));
    if (!previewsDirectory.exists()) {
        return {};
    }

    QVector<PreviewPackage> previews;
    const QStringList files = previewsDirectory.entryList({QStringLiteral("*.json")}, QDir::Files, QDir::Name);
    previews.reserve(files.size());

    for (const QString &fileName : files) {
        PreviewPackage preview;
        if (!loadPreviewFromFile(previewsDirectory.filePath(fileName), &preview, errorMessage)) {
            return {};
        }
        previews.push_back(preview);
    }

    std::sort(previews.begin(), previews.end(), [](const PreviewPackage &lhs, const PreviewPackage &rhs) {
        if (lhs.version != rhs.version) {
            return lhs.version > rhs.version;
        }

        return lhs.updatedAt > rhs.updatedAt;
    });

    return previews;
}

QVector<BuildJob> ProjectRepository::listJobs(const QString &projectId, QString *errorMessage) const
{
    setError(errorMessage, {});

    if (!migrateLegacyStoreIfNeeded(errorMessage)) {
        return {};
    }

    QDir jobsDirectory(jobsRootPath(projectId));
    if (!jobsDirectory.exists()) {
        return {};
    }

    QVector<BuildJob> jobs;
    const QStringList files = jobsDirectory.entryList({QStringLiteral("*.json")}, QDir::Files, QDir::Name);
    jobs.reserve(files.size());

    for (const QString &fileName : files) {
        BuildJob job;
        if (!loadJobFromFile(jobsDirectory.filePath(fileName), &job, errorMessage)) {
            return {};
        }
        jobs.push_back(job);
    }

    std::sort(jobs.begin(), jobs.end(), [](const BuildJob &lhs, const BuildJob &rhs) {
        return lhs.startedAt > rhs.startedAt;
    });

    return jobs;
}

QVector<ValidationReport> ProjectRepository::listValidationReports(const QString &projectId, QString *errorMessage) const
{
    setError(errorMessage, {});

    if (!migrateLegacyStoreIfNeeded(errorMessage)) {
        return {};
    }

    QDir validationsDirectory(validationsRootPath(projectId));
    if (!validationsDirectory.exists()) {
        return {};
    }

    QVector<ValidationReport> reports;
    const QStringList files = validationsDirectory.entryList({QStringLiteral("*.json")}, QDir::Files, QDir::Name);
    reports.reserve(files.size());

    for (const QString &fileName : files) {
        ValidationReport report;
        if (!loadValidationReportFromFile(validationsDirectory.filePath(fileName), &report, errorMessage)) {
            return {};
        }
        reports.push_back(report);
    }

    std::sort(reports.begin(), reports.end(), [](const ValidationReport &lhs, const ValidationReport &rhs) {
        return lhs.createdAt > rhs.createdAt;
    });

    return reports;
}

bool ProjectRepository::create(const ProjectDraft &draft, Project *createdProject, QString *errorMessage) const
{
    setError(errorMessage, {});

    if (!validateDraft(draft, createdProject, errorMessage)) {
        return false;
    }

    if (!migrateLegacyStoreIfNeeded(errorMessage)) {
        return false;
    }

    const Project project = Project::fromDraft(draft);
    if (!writeProject(project, errorMessage)) {
        return false;
    }

    *createdProject = project;
    return true;
}

bool ProjectRepository::createBlueprint(const QString &projectId, const BlueprintDraft &draft, EditorialBlueprint *createdBlueprint, QString *errorMessage) const
{
    setError(errorMessage, {});

    if (!validateBlueprintDraft(draft, createdBlueprint, errorMessage)) {
        return false;
    }

    if (!migrateLegacyStoreIfNeeded(errorMessage)) {
        return false;
    }

    Project project;
    if (!loadProject(projectId, &project, errorMessage)) {
        return false;
    }

    QString nestedError;
    const QVector<EditorialBlueprint> existingBlueprints = listBlueprints(projectId, &nestedError);
    if (!nestedError.isEmpty()) {
        setError(errorMessage, nestedError);
        return false;
    }

    EditorialBlueprint blueprint;
    blueprint.id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    blueprint.projectId = projectId;
    blueprint.version = existingBlueprints.isEmpty() ? 1 : existingBlueprints.front().version + 1;
    blueprint.chapterPlans = draft.chapterPlans;
    blueprint.terminologyRules = draft.terminologyRules.trimmed();
    blueprint.styleBible = draft.styleBible.trimmed();
    blueprint.crossReferencePlan = draft.crossReferencePlan.trimmed();
    blueprint.checklistPlan = draft.checklistPlan.trimmed();
    blueprint.visualPackIntent = draft.visualPackIntent.trimmed();
    blueprint.spoilerGuardrails = draft.spoilerGuardrails.trimmed();
    blueprint.createdAt = currentIsoTimestamp();
    blueprint.updatedAt = blueprint.createdAt;

    if (!writeBlueprint(blueprint, errorMessage)) {
        return false;
    }

    project.projectState = QStringLiteral("blueprint-ready");
    if (!writeProject(project, errorMessage)) {
        QFile::remove(blueprintPath(projectId, blueprint.id));
        return false;
    }

    *createdBlueprint = blueprint;
    return true;
}

bool ProjectRepository::createPreview(const QString &projectId, PreviewPackage *createdPreview, QString *errorMessage) const
{
    setError(errorMessage, {});

    if (createdPreview == nullptr) {
        setError(errorMessage, QStringLiteral("Internal error: missing preview output."));
        return false;
    }

    if (!migrateLegacyStoreIfNeeded(errorMessage)) {
        return false;
    }

    Project project;
    if (!loadProject(projectId, &project, errorMessage)) {
        return false;
    }

    QString nestedError;
    const QVector<EditorialBlueprint> blueprints = listBlueprints(projectId, &nestedError);
    if (!nestedError.isEmpty()) {
        setError(errorMessage, nestedError);
        return false;
    }
    if (blueprints.isEmpty()) {
        setError(errorMessage, QStringLiteral("Draft an editorial blueprint before generating a preview."));
        return false;
    }

    const QVector<PreviewPackage> existingPreviews = listPreviews(projectId, &nestedError);
    if (!nestedError.isEmpty()) {
        setError(errorMessage, nestedError);
        return false;
    }

    PreviewPackage preview = buildPreviewPackage(project, blueprints.front(), existingPreviews.isEmpty() ? 1 : existingPreviews.front().version + 1);
    if (!writePreview(preview, errorMessage)) {
        return false;
    }

    project.projectState = QStringLiteral("preview-ready");
    if (!writeProject(project, errorMessage)) {
        QFile::remove(previewPath(projectId, preview.id));
        return false;
    }

    *createdPreview = preview;
    return true;
}

bool ProjectRepository::reviewPreview(const QString &previewId, const QString &approvalState, const QString &reviewNotes, PreviewPackage *updatedPreview, QString *errorMessage) const
{
    setError(errorMessage, {});

    if (updatedPreview == nullptr) {
        setError(errorMessage, QStringLiteral("Internal error: missing preview output."));
        return false;
    }

    if (!validatePreviewApprovalState(approvalState, reviewNotes, errorMessage)) {
        return false;
    }

    if (!migrateLegacyStoreIfNeeded(errorMessage)) {
        return false;
    }

    PreviewPackage preview;
    if (!findPreviewById(previewId, &preview, errorMessage)) {
        return false;
    }

    Project project;
    if (!loadProject(preview.projectId, &project, errorMessage)) {
        return false;
    }

    const PreviewPackage originalPreview = preview;
    preview.approvalState = approvalState.trimmed();
    preview.reviewNotes = reviewNotes.trimmed();
    preview.updatedAt = currentIsoTimestamp();

    if (!writePreview(preview, errorMessage)) {
        return false;
    }

    project.projectState = approvalState.trimmed() == QStringLiteral("changes-requested")
        ? QStringLiteral("revision-requested")
        : QStringLiteral("preview-ready");

    if (!writeProject(project, errorMessage)) {
        QString rollbackError;
        writePreview(originalPreview, &rollbackError);
        return false;
    }

    *updatedPreview = preview;
    return true;
}

bool ProjectRepository::update(const QString &projectId, const ProjectDraft &draft, Project *updatedProject, QString *errorMessage) const
{
    setError(errorMessage, {});

    if (!validateDraft(draft, updatedProject, errorMessage)) {
        return false;
    }

    if (!migrateLegacyStoreIfNeeded(errorMessage)) {
        return false;
    }

    Project project;
    if (!loadProject(projectId, &project, errorMessage)) {
        return false;
    }

    applyDraft(project, draft);
    if (project.projectState.trimmed().isEmpty()) {
        project.projectState = QStringLiteral("intake");
    }

    if (!writeProject(project, errorMessage)) {
        return false;
    }

    *updatedProject = project;
    return true;
}

bool ProjectRepository::runIntakeValidation(const QString &projectId, QString *errorMessage) const
{
    setError(errorMessage, {});

    if (!migrateLegacyStoreIfNeeded(errorMessage)) {
        return false;
    }

    Project project;
    if (!loadProject(projectId, &project, errorMessage)) {
        return false;
    }

    const ValidationReport report = buildIntakeValidationReport(project);
    if (!writeValidationReport(report, errorMessage)) {
        return false;
    }

    BuildJob job;
    job.id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    job.projectId = projectId;
    job.jobType = QStringLiteral("validate");
    job.status = report.blockingCount > 0 ? QStringLiteral("blocked") : QStringLiteral("completed");
    job.progress = 100;
    job.startedAt = report.createdAt;
    job.endedAt = report.createdAt;
    job.humanSummary = report.summary;
    job.validationReportId = report.id;

    return writeJob(job, errorMessage);
}

bool ProjectRepository::remove(const QString &projectId, QString *errorMessage) const
{
    setError(errorMessage, {});

    if (!migrateLegacyStoreIfNeeded(errorMessage)) {
        return false;
    }

    QDir projectDirectory(projectRootPath(projectId));
    if (!projectDirectory.exists()) {
        setError(errorMessage, QStringLiteral("The selected project could not be found."));
        return false;
    }

    if (!projectDirectory.removeRecursively()) {
        setError(errorMessage, QStringLiteral("Could not remove project workspace %1.").arg(projectDirectory.path()));
        return false;
    }

    return true;
}

bool ProjectRepository::findPreviewById(const QString &previewId, PreviewPackage *preview, QString *errorMessage) const
{
    setError(errorMessage, {});

    if (preview == nullptr) {
        setError(errorMessage, QStringLiteral("Internal error: missing preview output."));
        return false;
    }

    if (!migrateLegacyStoreIfNeeded(errorMessage)) {
        return false;
    }

    QDir projectsDirectory(projectsRootPath());
    if (!projectsDirectory.exists()) {
        setError(errorMessage, QStringLiteral("Preview not found: %1").arg(previewId));
        return false;
    }

    const QStringList projectIds = projectsDirectory.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
    for (const QString &projectId : projectIds) {
        const QString filePath = previewPath(projectId, previewId);
        if (!QFileInfo::exists(filePath)) {
            continue;
        }

        return loadPreviewFromFile(filePath, preview, errorMessage);
    }

    setError(errorMessage, QStringLiteral("Preview not found: %1").arg(previewId));
    return false;
}

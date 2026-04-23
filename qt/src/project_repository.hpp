#pragma once

#include "project.hpp"

#include <QVector>

class ProjectRepository {
public:
    explicit ProjectRepository(QString storageRoot = {});

    [[nodiscard]] QString storageRootPath() const;
    [[nodiscard]] QString projectsRootPath() const;
    [[nodiscard]] QString projectRootPath(const QString &projectId) const;
    [[nodiscard]] QVector<Project> all(QString *errorMessage = nullptr) const;
    [[nodiscard]] QVector<EditorialBlueprint> listBlueprints(const QString &projectId, QString *errorMessage = nullptr) const;
    [[nodiscard]] QVector<GuideBuildBundle> listBuildBundles(const QString &projectId, QString *errorMessage = nullptr) const;
    [[nodiscard]] QVector<KnowledgeEntity> listKnowledgeEntities(const QString &projectId, QString *errorMessage = nullptr) const;
    [[nodiscard]] QVector<PreviewPackage> listPreviews(const QString &projectId, QString *errorMessage = nullptr) const;
    [[nodiscard]] QVector<BuildJob> listJobs(const QString &projectId, QString *errorMessage = nullptr) const;
    [[nodiscard]] QVector<ValidationReport> listValidationReports(const QString &projectId, QString *errorMessage = nullptr) const;
    bool create(const ProjectDraft &draft, Project *createdProject, QString *errorMessage = nullptr) const;
    bool createBlueprint(const QString &projectId, const BlueprintDraft &draft, EditorialBlueprint *createdBlueprint, QString *errorMessage = nullptr) const;
    bool createKnowledgeEntity(const QString &projectId, const KnowledgeEntityDraft &draft, KnowledgeEntity *createdEntity, QString *errorMessage = nullptr) const;
    bool updateKnowledgeEntity(const QString &entityId, const KnowledgeEntityDraft &draft, KnowledgeEntity *updatedEntity, QString *errorMessage = nullptr) const;
    bool createPreview(const QString &projectId, PreviewPackage *createdPreview, QString *errorMessage = nullptr) const;
    bool reviewPreview(const QString &previewId, const QString &approvalState, const QString &reviewNotes, PreviewPackage *updatedPreview, QString *errorMessage = nullptr) const;
    bool createFullBuild(const QString &projectId, GuideBuildBundle *createdBundle, QString *errorMessage = nullptr) const;
    bool update(const QString &projectId, const ProjectDraft &draft, Project *updatedProject, QString *errorMessage = nullptr) const;
    bool runIntakeValidation(const QString &projectId, QString *errorMessage = nullptr) const;
    bool remove(const QString &projectId, QString *errorMessage = nullptr) const;

private:
    [[nodiscard]] QString resolvedStorageRoot() const;
    [[nodiscard]] QString legacyProjectsFilePath() const;
    [[nodiscard]] QString legacyBackupFilePath() const;
    [[nodiscard]] QString projectManifestPath(const QString &projectId) const;
    [[nodiscard]] QString blueprintsRootPath(const QString &projectId) const;
    [[nodiscard]] QString blueprintPath(const QString &projectId, const QString &blueprintId) const;
    [[nodiscard]] QString buildBundlesRootPath(const QString &projectId) const;
    [[nodiscard]] QString buildBundlePath(const QString &projectId, const QString &bundleId) const;
    [[nodiscard]] QString knowledgeRootPath(const QString &projectId) const;
    [[nodiscard]] QString knowledgePath(const QString &projectId, const QString &entityId) const;
    [[nodiscard]] QString previewsRootPath(const QString &projectId) const;
    [[nodiscard]] QString previewPath(const QString &projectId, const QString &previewId) const;
    [[nodiscard]] QString jobsRootPath(const QString &projectId) const;
    [[nodiscard]] QString jobPath(const QString &projectId, const QString &jobId) const;
    [[nodiscard]] QString validationsRootPath(const QString &projectId) const;
    [[nodiscard]] QString validationReportPath(const QString &projectId, const QString &reportId) const;
    bool ensureProjectsRootExists(QString *errorMessage = nullptr) const;
    bool ensureProjectLayout(const QString &projectId, QString *errorMessage = nullptr) const;
    bool writeProject(const Project &project, QString *errorMessage = nullptr) const;
    bool writeBlueprint(const EditorialBlueprint &blueprint, QString *errorMessage = nullptr) const;
    bool writeBuildBundle(const GuideBuildBundle &bundle, QString *errorMessage = nullptr) const;
    bool writeKnowledgeEntity(const KnowledgeEntity &entity, QString *errorMessage = nullptr) const;
    bool writePreview(const PreviewPackage &preview, QString *errorMessage = nullptr) const;
    bool writeJob(const BuildJob &job, QString *errorMessage = nullptr) const;
    bool writeValidationReport(const ValidationReport &report, QString *errorMessage = nullptr) const;
    bool loadProject(const QString &projectId, Project *project, QString *errorMessage = nullptr) const;
    bool loadProjectFromFile(const QString &filePath, Project *project, QString *errorMessage = nullptr) const;
    bool loadBlueprintFromFile(const QString &filePath, EditorialBlueprint *blueprint, QString *errorMessage = nullptr) const;
    bool loadBuildBundleFromFile(const QString &filePath, GuideBuildBundle *bundle, QString *errorMessage = nullptr) const;
    bool loadKnowledgeEntityFromFile(const QString &filePath, KnowledgeEntity *entity, QString *errorMessage = nullptr) const;
    bool loadPreviewFromFile(const QString &filePath, PreviewPackage *preview, QString *errorMessage = nullptr) const;
    bool loadJobFromFile(const QString &filePath, BuildJob *job, QString *errorMessage = nullptr) const;
    bool loadValidationReportFromFile(const QString &filePath, ValidationReport *report, QString *errorMessage = nullptr) const;
    bool findKnowledgeEntityById(const QString &entityId, KnowledgeEntity *entity, QString *errorMessage = nullptr) const;
    bool findPreviewById(const QString &previewId, PreviewPackage *preview, QString *errorMessage = nullptr) const;
    bool migrateLegacyStoreIfNeeded(QString *errorMessage = nullptr) const;

    QString m_storageRoot;
};

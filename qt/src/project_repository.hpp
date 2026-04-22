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
    [[nodiscard]] QVector<BuildJob> listJobs(const QString &projectId, QString *errorMessage = nullptr) const;
    [[nodiscard]] QVector<ValidationReport> listValidationReports(const QString &projectId, QString *errorMessage = nullptr) const;
    bool create(const ProjectDraft &draft, Project *createdProject, QString *errorMessage = nullptr) const;
    bool createBlueprint(const QString &projectId, const BlueprintDraft &draft, EditorialBlueprint *createdBlueprint, QString *errorMessage = nullptr) const;
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
    [[nodiscard]] QString jobsRootPath(const QString &projectId) const;
    [[nodiscard]] QString jobPath(const QString &projectId, const QString &jobId) const;
    [[nodiscard]] QString validationsRootPath(const QString &projectId) const;
    [[nodiscard]] QString validationReportPath(const QString &projectId, const QString &reportId) const;
    bool ensureProjectsRootExists(QString *errorMessage = nullptr) const;
    bool ensureProjectLayout(const QString &projectId, QString *errorMessage = nullptr) const;
    bool writeProject(const Project &project, QString *errorMessage = nullptr) const;
    bool writeBlueprint(const EditorialBlueprint &blueprint, QString *errorMessage = nullptr) const;
    bool writeJob(const BuildJob &job, QString *errorMessage = nullptr) const;
    bool writeValidationReport(const ValidationReport &report, QString *errorMessage = nullptr) const;
    bool loadProject(const QString &projectId, Project *project, QString *errorMessage = nullptr) const;
    bool loadProjectFromFile(const QString &filePath, Project *project, QString *errorMessage = nullptr) const;
    bool loadBlueprintFromFile(const QString &filePath, EditorialBlueprint *blueprint, QString *errorMessage = nullptr) const;
    bool loadJobFromFile(const QString &filePath, BuildJob *job, QString *errorMessage = nullptr) const;
    bool loadValidationReportFromFile(const QString &filePath, ValidationReport *report, QString *errorMessage = nullptr) const;
    bool migrateLegacyStoreIfNeeded(QString *errorMessage = nullptr) const;

    QString m_storageRoot;
};

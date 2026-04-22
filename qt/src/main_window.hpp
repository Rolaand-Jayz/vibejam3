#pragma once

#include "project.hpp"
#include "project_repository.hpp"

#include <QMainWindow>
#include <QVector>

class QLabel;
class QListWidget;
class QListWidgetItem;
class QPushButton;
class QStackedWidget;
class QWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void createProject();
    void editSelectedProject();
    void draftBlueprintForSelectedProject();
    void previewSelectedProject();
    void runIntakeCheck();
    void deleteSelectedProject();
    void refreshProjects();
    void handleSelectionChanged();

private:
    void buildUi();
    void populateProjectList(const QString &selectedProjectId = {});
    void showEmptyState();
    void showProject(const Project &project);
    void selectProject(const QString &projectId);
    [[nodiscard]] QString currentProjectId() const;
    [[nodiscard]] const Project *findProject(const QString &projectId) const;
    void updateStatusBar();

    ProjectRepository m_repository;
    QVector<Project> m_projects;

    QListWidget *m_projectList = nullptr;
    QStackedWidget *m_detailStack = nullptr;
    QWidget *m_emptyPage = nullptr;
    QWidget *m_detailPage = nullptr;
    QPushButton *m_editButton = nullptr;
    QPushButton *m_blueprintButton = nullptr;
    QPushButton *m_previewButton = nullptr;
    QPushButton *m_validateButton = nullptr;
    QPushButton *m_deleteButton = nullptr;

    QLabel *m_detailTitle = nullptr;
    QLabel *m_detailSummary = nullptr;
    QLabel *m_stateValue = nullptr;
    QLabel *m_platformValue = nullptr;
    QLabel *m_guideIntentValue = nullptr;
    QLabel *m_playerGoalValue = nullptr;
    QLabel *m_runStyleValue = nullptr;
    QLabel *m_spoilerPolicyValue = nullptr;
    QLabel *m_depthPreferenceValue = nullptr;
    QLabel *m_unresolvedValue = nullptr;
    QLabel *m_validationStatusValue = nullptr;
    QLabel *m_validationSummaryValue = nullptr;
    QLabel *m_lastCheckedValue = nullptr;
    QLabel *m_themeCssValue = nullptr;
    QLabel *m_blueprintSummaryValue = nullptr;
    QLabel *m_blueprintChaptersValue = nullptr;
    QLabel *m_blueprintUpdatedAtValue = nullptr;
    QLabel *m_previewSummaryValue = nullptr;
    QLabel *m_previewApprovalValue = nullptr;
    QLabel *m_previewSectionsValue = nullptr;
    QLabel *m_previewUpdatedAtValue = nullptr;
    QLabel *m_createdAtValue = nullptr;
    QLabel *m_identifierValue = nullptr;

    QLabel *m_lensTitleValue = nullptr;
    QLabel *m_lensStateValue = nullptr;
    QLabel *m_lensRunStyleValue = nullptr;
    QLabel *m_lensSpoilersValue = nullptr;
    QLabel *m_lensDepthValue = nullptr;
    QLabel *m_lensUnresolvedValue = nullptr;
    QLabel *m_lensValidationStateValue = nullptr;
    QLabel *m_lensValidationSummaryValue = nullptr;
    QLabel *m_lensThemeCssValue = nullptr;
    QLabel *m_lensBlueprintValue = nullptr;
    QLabel *m_lensPreviewValue = nullptr;
    QLabel *m_lensPreviewApprovalValue = nullptr;
    QLabel *m_storagePathValue = nullptr;
};

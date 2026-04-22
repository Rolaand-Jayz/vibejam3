#include "main_window.hpp"

#include "blueprint_dialog.hpp"
#include "new_project_dialog.hpp"
#include "preview_review_dialog.hpp"

#include <QAction>
#include <QDialog>
#include <QFormLayout>
#include <QFrame>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QSplitter>
#include <QStackedWidget>
#include <QStatusBar>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWidget>

namespace {
QFrame *createPanelCard(QWidget *parent = nullptr)
{
    auto *frame = new QFrame(parent);
    frame->setObjectName(QStringLiteral("panelCard"));
    frame->setFrameShape(QFrame::NoFrame);
    return frame;
}

QLabel *createValueLabel(QWidget *parent = nullptr)
{
    auto *label = new QLabel(parent);
    label->setWordWrap(true);
    label->setTextInteractionFlags(Qt::TextSelectableByMouse);
    return label;
}

QLabel *createBadgeLabel(QWidget *parent = nullptr)
{
    auto *label = new QLabel(parent);
    label->setObjectName(QStringLiteral("stateBadge"));
    label->setAlignment(Qt::AlignCenter);
    label->setMargin(6);
    return label;
}

QLabel *createSectionTitle(const QString &text, QWidget *parent = nullptr)
{
    auto *label = new QLabel(text, parent);
    label->setObjectName(QStringLiteral("sectionTitle"));
    return label;
}

BlueprintDraft createBlueprintDraftForProject(const Project &project, const EditorialBlueprint *latestBlueprint)
{
    if (latestBlueprint != nullptr) {
        return latestBlueprint->toDraft();
    }

    const QString playerGoal = project.playerGoal.trimmed().isEmpty()
        ? QStringLiteral("the chosen run goal")
        : project.playerGoal.trimmed();

    return BlueprintDraft {
        .chapterPlans = {
            BlueprintChapterPlan {
                QStringLiteral("Opening route"),
                QStringLiteral("Establish the early-game setup, route constraints, and immediate warnings for %1.").arg(project.displayTitle())
            },
            BlueprintChapterPlan {
                QStringLiteral("Core progression"),
                QStringLiteral("Lay out the main route, power spikes, and decision points for a %1 run.").arg(project.runStyle.trimmed().isEmpty() ? QStringLiteral("focused") : project.runStyle.trimmed())
            },
            BlueprintChapterPlan {
                QStringLiteral("Cleanup and ending locks"),
                QStringLiteral("Track optional cleanup, missables, and end-state requirements tied to %1.").arg(playerGoal)
            },
        },
        .terminologyRules = QStringLiteral("Use official in-game names for locations, systems, and boss labels wherever source certainty is high."),
        .styleBible = QStringLiteral("Write like a premium field manual: calm, tactical, specific, and %1 about story reveals.").arg(project.spoilerPolicy.trimmed().isEmpty() ? QStringLiteral("minimal") : project.spoilerPolicy.trimmed()),
        .crossReferencePlan = QStringLiteral("Link route-critical warnings back to reference notes, missable checkpoints, and optional detour sections."),
        .checklistPlan = QStringLiteral("End each planned chapter with irreversible checks, recommended prep, and a short before-you-leave checklist."),
        .visualPackIntent = QStringLiteral("Keep the surface editorial and book-like: restrained callouts, warm accents, and compact tactical scannability."),
        .spoilerGuardrails = QStringLiteral("Keep plot reveals out of headings, front matter, and route summaries unless the chosen spoiler policy explicitly allows them."),
    };
}

QString describePreview(const PreviewPackage *latestPreview, const EditorialBlueprint *latestBlueprint)
{
    if (latestPreview == nullptr) {
        return QStringLiteral("No preview package generated yet.");
    }

    if (latestBlueprint != nullptr && latestPreview->blueprintVersion != latestBlueprint->version) {
        return QStringLiteral("Preview v%1 targets blueprint v%2; generate a fresh preview for blueprint v%3.")
            .arg(latestPreview->version)
            .arg(latestPreview->blueprintVersion)
            .arg(latestBlueprint->version);
    }

    return QStringLiteral("Preview v%1 is ready for review against blueprint v%2.")
        .arg(latestPreview->version)
        .arg(latestPreview->blueprintVersion);
}

bool previewMatchesLatestBlueprint(const PreviewPackage *latestPreview, const EditorialBlueprint *latestBlueprint)
{
    return latestPreview != nullptr && latestBlueprint != nullptr && latestPreview->blueprintVersion == latestBlueprint->version;
}
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(QStringLiteral("MY PLAITHROUGH — Guide Workbench"));
    resize(1480, 920);
    setMinimumSize(1180, 760);
    buildUi();
    refreshProjects();
}

void MainWindow::buildUi()
{
    auto *primaryToolbar = addToolBar(QStringLiteral("Primary"));
    primaryToolbar->setMovable(false);
    primaryToolbar->setFloatable(false);

    QAction *newAction = primaryToolbar->addAction(QStringLiteral("New Project"));
    QAction *editAction = primaryToolbar->addAction(QStringLiteral("Edit Project"));
    QAction *blueprintAction = primaryToolbar->addAction(QStringLiteral("Draft Blueprint"));
    QAction *previewAction = primaryToolbar->addAction(QStringLiteral("Preview / Review"));
    QAction *validateAction = primaryToolbar->addAction(QStringLiteral("Check Intake"));
    QAction *refreshAction = primaryToolbar->addAction(QStringLiteral("Refresh"));
    QAction *deleteAction = primaryToolbar->addAction(QStringLiteral("Delete"));

    connect(newAction, &QAction::triggered, this, &MainWindow::createProject);
    connect(editAction, &QAction::triggered, this, &MainWindow::editSelectedProject);
    connect(blueprintAction, &QAction::triggered, this, &MainWindow::draftBlueprintForSelectedProject);
    connect(previewAction, &QAction::triggered, this, &MainWindow::previewSelectedProject);
    connect(validateAction, &QAction::triggered, this, &MainWindow::runIntakeCheck);
    connect(refreshAction, &QAction::triggered, this, &MainWindow::refreshProjects);
    connect(deleteAction, &QAction::triggered, this, &MainWindow::deleteSelectedProject);

    auto *root = new QWidget(this);
    auto *rootLayout = new QHBoxLayout(root);
    rootLayout->setContentsMargins(18, 18, 18, 18);
    rootLayout->setSpacing(18);

    auto *splitter = new QSplitter(Qt::Horizontal, root);
    splitter->setChildrenCollapsible(false);
    rootLayout->addWidget(splitter);

    auto *leftRail = createPanelCard(splitter);
    auto *leftLayout = new QVBoxLayout(leftRail);
    leftLayout->setContentsMargins(18, 18, 18, 18);
    leftLayout->setSpacing(14);

    auto *appTitle = new QLabel(QStringLiteral("MY PLAITHROUGH"), leftRail);
    appTitle->setObjectName(QStringLiteral("heroTitle"));
    auto *appSubtitle = new QLabel(QStringLiteral("Editorial desktop shell for building personal game guides."), leftRail);
    appSubtitle->setObjectName(QStringLiteral("heroSubtitle"));
    appSubtitle->setWordWrap(true);

    auto *buttonRow = new QHBoxLayout();
    buttonRow->setSpacing(10);
    auto *newButton = new QPushButton(QStringLiteral("New"), leftRail);
    newButton->setObjectName(QStringLiteral("primaryButton"));
    m_editButton = new QPushButton(QStringLiteral("Edit"), leftRail);
    m_blueprintButton = new QPushButton(QStringLiteral("Draft Blueprint"), leftRail);
    m_previewButton = new QPushButton(QStringLiteral("Preview / Review"), leftRail);
    m_validateButton = new QPushButton(QStringLiteral("Check Intake"), leftRail);
    m_deleteButton = new QPushButton(QStringLiteral("Delete"), leftRail);
    buttonRow->addWidget(newButton);
    buttonRow->addWidget(m_editButton);
    buttonRow->addWidget(m_blueprintButton);
    buttonRow->addWidget(m_previewButton);
    buttonRow->addWidget(m_validateButton);
    buttonRow->addWidget(m_deleteButton);

    m_projectList = new QListWidget(leftRail);
    m_projectList->setSelectionMode(QAbstractItemView::SingleSelection);
    m_projectList->setAlternatingRowColors(false);
    m_projectList->setUniformItemSizes(false);

    leftLayout->addWidget(appTitle);
    leftLayout->addWidget(appSubtitle);
    leftLayout->addLayout(buttonRow);
    leftLayout->addWidget(createSectionTitle(QStringLiteral("Guide projects"), leftRail));
    leftLayout->addWidget(m_projectList, 1);

    connect(newButton, &QPushButton::clicked, this, &MainWindow::createProject);
    connect(m_editButton, &QPushButton::clicked, this, &MainWindow::editSelectedProject);
    connect(m_blueprintButton, &QPushButton::clicked, this, &MainWindow::draftBlueprintForSelectedProject);
    connect(m_previewButton, &QPushButton::clicked, this, &MainWindow::previewSelectedProject);
    connect(m_validateButton, &QPushButton::clicked, this, &MainWindow::runIntakeCheck);
    connect(m_deleteButton, &QPushButton::clicked, this, &MainWindow::deleteSelectedProject);
    connect(m_projectList, &QListWidget::itemSelectionChanged, this, &MainWindow::handleSelectionChanged);

    auto *centerCard = createPanelCard(splitter);
    auto *centerLayout = new QVBoxLayout(centerCard);
    centerLayout->setContentsMargins(22, 22, 22, 22);
    centerLayout->setSpacing(16);

    m_detailStack = new QStackedWidget(centerCard);

    m_emptyPage = new QWidget(m_detailStack);
    auto *emptyLayout = new QVBoxLayout(m_emptyPage);
    emptyLayout->setContentsMargins(24, 24, 24, 24);
    emptyLayout->setSpacing(12);
    auto *emptyTitle = new QLabel(QStringLiteral("No guide projects yet"), m_emptyPage);
    emptyTitle->setObjectName(QStringLiteral("heroTitle"));
    auto *emptyBody = new QLabel(QStringLiteral("Create a project to begin shaping a personal guide, playthrough, or reference build."), m_emptyPage);
    emptyBody->setWordWrap(true);
    emptyBody->setObjectName(QStringLiteral("heroSubtitle"));
    emptyLayout->addStretch();
    emptyLayout->addWidget(emptyTitle);
    emptyLayout->addWidget(emptyBody);
    emptyLayout->addStretch();

    m_detailPage = new QWidget(m_detailStack);
    auto *detailLayout = new QVBoxLayout(m_detailPage);
    detailLayout->setContentsMargins(8, 8, 8, 8);
    detailLayout->setSpacing(14);

    m_detailTitle = new QLabel(m_detailPage);
    m_detailTitle->setObjectName(QStringLiteral("heroTitle"));
    m_detailSummary = new QLabel(m_detailPage);
    m_detailSummary->setObjectName(QStringLiteral("heroSubtitle"));
    m_detailSummary->setWordWrap(true);

    auto *detailForm = new QFormLayout();
    detailForm->setLabelAlignment(Qt::AlignLeft | Qt::AlignTop);
    detailForm->setFormAlignment(Qt::AlignTop);
    detailForm->setHorizontalSpacing(18);
    detailForm->setVerticalSpacing(12);

    m_stateValue = createBadgeLabel(m_detailPage);
    m_platformValue = createValueLabel(m_detailPage);
    m_guideIntentValue = createValueLabel(m_detailPage);
    m_playerGoalValue = createValueLabel(m_detailPage);
    m_runStyleValue = createValueLabel(m_detailPage);
    m_spoilerPolicyValue = createValueLabel(m_detailPage);
    m_depthPreferenceValue = createValueLabel(m_detailPage);
    m_unresolvedValue = createValueLabel(m_detailPage);
    m_validationStatusValue = createBadgeLabel(m_detailPage);
    m_validationSummaryValue = createValueLabel(m_detailPage);
    m_lastCheckedValue = createValueLabel(m_detailPage);
    m_themeCssValue = createValueLabel(m_detailPage);
    m_blueprintSummaryValue = createValueLabel(m_detailPage);
    m_blueprintChaptersValue = createValueLabel(m_detailPage);
    m_blueprintUpdatedAtValue = createValueLabel(m_detailPage);
    m_previewSummaryValue = createValueLabel(m_detailPage);
    m_previewApprovalValue = createBadgeLabel(m_detailPage);
    m_previewSectionsValue = createValueLabel(m_detailPage);
    m_previewUpdatedAtValue = createValueLabel(m_detailPage);
    m_createdAtValue = createValueLabel(m_detailPage);
    m_identifierValue = createValueLabel(m_detailPage);

    detailForm->addRow(QStringLiteral("State"), m_stateValue);
    detailForm->addRow(QStringLiteral("Platform / version"), m_platformValue);
    detailForm->addRow(QStringLiteral("Guide intent"), m_guideIntentValue);
    detailForm->addRow(QStringLiteral("Player goal"), m_playerGoalValue);
    detailForm->addRow(QStringLiteral("Run style"), m_runStyleValue);
    detailForm->addRow(QStringLiteral("Spoiler policy"), m_spoilerPolicyValue);
    detailForm->addRow(QStringLiteral("Depth preference"), m_depthPreferenceValue);
    detailForm->addRow(QStringLiteral("Unresolved"), m_unresolvedValue);
    detailForm->addRow(QStringLiteral("Validation"), m_validationStatusValue);
    detailForm->addRow(QStringLiteral("Validation summary"), m_validationSummaryValue);
    detailForm->addRow(QStringLiteral("Last checked"), m_lastCheckedValue);
    detailForm->addRow(QStringLiteral("Shared web theme"), m_themeCssValue);
    detailForm->addRow(QStringLiteral("Editorial blueprint"), m_blueprintSummaryValue);
    detailForm->addRow(QStringLiteral("Blueprint chapters"), m_blueprintChaptersValue);
    detailForm->addRow(QStringLiteral("Blueprint updated"), m_blueprintUpdatedAtValue);
    detailForm->addRow(QStringLiteral("Preview package"), m_previewSummaryValue);
    detailForm->addRow(QStringLiteral("Preview approval"), m_previewApprovalValue);
    detailForm->addRow(QStringLiteral("Preview sections"), m_previewSectionsValue);
    detailForm->addRow(QStringLiteral("Preview updated"), m_previewUpdatedAtValue);
    detailForm->addRow(QStringLiteral("Created"), m_createdAtValue);
    detailForm->addRow(QStringLiteral("Identifier"), m_identifierValue);

    detailLayout->addWidget(m_detailTitle);
    detailLayout->addWidget(m_detailSummary);
    detailLayout->addSpacing(6);
    detailLayout->addLayout(detailForm);
    detailLayout->addStretch();

    m_detailStack->addWidget(m_emptyPage);
    m_detailStack->addWidget(m_detailPage);
    centerLayout->addWidget(m_detailStack);

    auto *rightRail = createPanelCard(splitter);
    auto *rightLayout = new QVBoxLayout(rightRail);
    rightLayout->setContentsMargins(18, 18, 18, 18);
    rightLayout->setSpacing(16);

    auto *lensTitle = createSectionTitle(QStringLiteral("Project lens"), rightRail);
    auto *lensSubtitle = new QLabel(QStringLiteral("A quick read on the selected guide’s tone and storage state."), rightRail);
    lensSubtitle->setObjectName(QStringLiteral("heroSubtitle"));
    lensSubtitle->setWordWrap(true);

    auto *lensBox = new QGroupBox(QStringLiteral("Current selection"), rightRail);
    auto *lensForm = new QFormLayout(lensBox);
    lensForm->setLabelAlignment(Qt::AlignLeft | Qt::AlignTop);
    lensForm->setFormAlignment(Qt::AlignTop);
    lensForm->setHorizontalSpacing(14);
    lensForm->setVerticalSpacing(12);

    m_lensTitleValue = createValueLabel(lensBox);
    m_lensStateValue = createBadgeLabel(lensBox);
    m_lensRunStyleValue = createValueLabel(lensBox);
    m_lensSpoilersValue = createValueLabel(lensBox);
    m_lensDepthValue = createValueLabel(lensBox);
    m_lensUnresolvedValue = createValueLabel(lensBox);
    m_lensValidationStateValue = createBadgeLabel(lensBox);
    m_lensValidationSummaryValue = createValueLabel(lensBox);
    m_lensThemeCssValue = createValueLabel(lensBox);
    m_lensBlueprintValue = createValueLabel(lensBox);
    m_lensPreviewValue = createValueLabel(lensBox);
    m_lensPreviewApprovalValue = createBadgeLabel(lensBox);
    m_storagePathValue = createValueLabel(lensBox);

    lensForm->addRow(QStringLiteral("Title"), m_lensTitleValue);
    lensForm->addRow(QStringLiteral("State"), m_lensStateValue);
    lensForm->addRow(QStringLiteral("Run style"), m_lensRunStyleValue);
    lensForm->addRow(QStringLiteral("Spoilers"), m_lensSpoilersValue);
    lensForm->addRow(QStringLiteral("Depth"), m_lensDepthValue);
    lensForm->addRow(QStringLiteral("Unresolved"), m_lensUnresolvedValue);
    lensForm->addRow(QStringLiteral("Validation"), m_lensValidationStateValue);
    lensForm->addRow(QStringLiteral("Summary"), m_lensValidationSummaryValue);
    lensForm->addRow(QStringLiteral("Theme"), m_lensThemeCssValue);
    lensForm->addRow(QStringLiteral("Blueprint"), m_lensBlueprintValue);
    lensForm->addRow(QStringLiteral("Preview"), m_lensPreviewValue);
    lensForm->addRow(QStringLiteral("Approval"), m_lensPreviewApprovalValue);
    lensForm->addRow(QStringLiteral("Store"), m_storagePathValue);

    rightLayout->addWidget(lensTitle);
    rightLayout->addWidget(lensSubtitle);
    rightLayout->addWidget(lensBox);
    rightLayout->addStretch();

    splitter->setStretchFactor(0, 2);
    splitter->setStretchFactor(1, 4);
    splitter->setStretchFactor(2, 2);

    setCentralWidget(root);
    statusBar()->setSizeGripEnabled(false);
}

void MainWindow::createProject()
{
    NewProjectDialog dialog(this);
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    Project createdProject;
    QString errorMessage;
    if (!m_repository.create(dialog.draft(), &createdProject, &errorMessage)) {
        QMessageBox::critical(this, QStringLiteral("Could not create project"), errorMessage);
        return;
    }

    refreshProjects();
    selectProject(createdProject.id);
    statusBar()->showMessage(QStringLiteral("Created %1").arg(createdProject.displayTitle()), 4000);
}

void MainWindow::editSelectedProject()
{
    const QString projectId = currentProjectId();
    const Project *project = findProject(projectId);
    if (project == nullptr) {
        return;
    }

    NewProjectDialog dialog(this);
    dialog.setDialogMode(
        QStringLiteral("Edit Project Manifest"),
        QStringLiteral("Refine the intake manifest while keeping the project identity and current intake state intact."),
        QStringLiteral("Save changes"));
    dialog.setDraft(project->toDraft());

    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    Project updatedProject;
    QString errorMessage;
    if (!m_repository.update(projectId, dialog.draft(), &updatedProject, &errorMessage)) {
        QMessageBox::critical(this, QStringLiteral("Could not update project"), errorMessage);
        return;
    }

    refreshProjects();
    selectProject(updatedProject.id);
    statusBar()->showMessage(QStringLiteral("Updated %1").arg(updatedProject.displayTitle()), 4000);
}

void MainWindow::draftBlueprintForSelectedProject()
{
    const QString projectId = currentProjectId();
    const Project *project = findProject(projectId);
    if (project == nullptr) {
        return;
    }

    QString errorMessage;
    const QVector<EditorialBlueprint> existingBlueprints = m_repository.listBlueprints(projectId, &errorMessage);
    if (!errorMessage.isEmpty()) {
        QMessageBox::critical(this, QStringLiteral("Could not load blueprints"), errorMessage);
        return;
    }

    const EditorialBlueprint *latestBlueprint = existingBlueprints.isEmpty() ? nullptr : &existingBlueprints.front();

    BlueprintDialog dialog(this);
    dialog.setDialogMode(
        latestBlueprint == nullptr ? QStringLiteral("Draft Editorial Blueprint") : QStringLiteral("Revise Editorial Blueprint"),
        latestBlueprint == nullptr
            ? QStringLiteral("Create the durable editorial plan that preview and full-build work will later follow.")
            : QStringLiteral("Save a new blueprint version so the guide plan stays durable and inspectable over time."),
        latestBlueprint == nullptr ? QStringLiteral("Save blueprint") : QStringLiteral("Save revision"));
    dialog.setDraft(createBlueprintDraftForProject(*project, latestBlueprint));

    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    const QString projectTitle = project->displayTitle();
    EditorialBlueprint createdBlueprint;
    if (!m_repository.createBlueprint(projectId, dialog.draft(), &createdBlueprint, &errorMessage)) {
        QMessageBox::critical(this, QStringLiteral("Could not save blueprint"), errorMessage);
        return;
    }

    refreshProjects();
    selectProject(projectId);
    statusBar()->showMessage(QStringLiteral("Saved blueprint v%1 for %2").arg(createdBlueprint.version).arg(projectTitle), 4000);
}

void MainWindow::previewSelectedProject()
{
    const QString projectId = currentProjectId();
    const Project *project = findProject(projectId);
    if (project == nullptr) {
        return;
    }

    const QString projectTitle = project->displayTitle();

    QString errorMessage;
    const QVector<EditorialBlueprint> existingBlueprints = m_repository.listBlueprints(projectId, &errorMessage);
    if (!errorMessage.isEmpty()) {
        QMessageBox::critical(this, QStringLiteral("Could not load blueprints"), errorMessage);
        return;
    }

    if (existingBlueprints.isEmpty()) {
        QMessageBox::information(
            this,
            QStringLiteral("Blueprint required"),
            QStringLiteral("Draft an editorial blueprint before generating a preview package."));
        return;
    }

    const EditorialBlueprint *latestBlueprint = &existingBlueprints.front();

    const QVector<PreviewPackage> existingPreviews = m_repository.listPreviews(projectId, &errorMessage);
    if (!errorMessage.isEmpty()) {
        QMessageBox::critical(this, QStringLiteral("Could not load previews"), errorMessage);
        return;
    }

    const PreviewPackage *latestPreview = existingPreviews.isEmpty() ? nullptr : &existingPreviews.front();
    const bool shouldGeneratePreview = latestPreview == nullptr
        || !previewMatchesLatestBlueprint(latestPreview, latestBlueprint)
        || latestPreview->approvalState == QStringLiteral("changes-requested")
        || project->stateLabel() == QStringLiteral("blueprint-ready");

    PreviewPackage workingPreview;
    if (shouldGeneratePreview) {
        if (!m_repository.createPreview(projectId, &workingPreview, &errorMessage)) {
            QMessageBox::critical(this, QStringLiteral("Could not generate preview"), errorMessage);
            return;
        }

        refreshProjects();
        selectProject(projectId);
    } else {
        workingPreview = *latestPreview;
    }

    PreviewReviewDialog dialog(this);
    dialog.setDialogMode(
        shouldGeneratePreview ? QStringLiteral("Review Preview Package") : QStringLiteral("Latest Preview Package"),
        shouldGeneratePreview
            ? QStringLiteral("A new durable preview was generated from the current blueprint. Review it before later full-build work continues.")
            : QStringLiteral("Review the latest durable preview and record an explicit approval decision if you are ready."));
    dialog.setPreview(projectTitle, workingPreview);

    if (dialog.exec() != QDialog::Accepted) {
        if (shouldGeneratePreview) {
            statusBar()->showMessage(QStringLiteral("Generated preview v%1 for %2").arg(workingPreview.version).arg(projectTitle), 4000);
        }
        refreshProjects();
        selectProject(projectId);
        return;
    }

    PreviewPackage reviewedPreview;
    if (!m_repository.reviewPreview(workingPreview.id, dialog.approvalState(), dialog.reviewNotes(), &reviewedPreview, &errorMessage)) {
        QMessageBox::critical(this, QStringLiteral("Could not record preview review"), errorMessage);
        return;
    }

    refreshProjects();
    selectProject(projectId);
    statusBar()->showMessage(
        dialog.approvalState() == QStringLiteral("approved")
            ? QStringLiteral("Approved preview v%1 for %2").arg(reviewedPreview.version).arg(projectTitle)
            : QStringLiteral("Marked preview v%1 for revision on %2").arg(reviewedPreview.version).arg(projectTitle),
        4000);
}

void MainWindow::runIntakeCheck()
{
    const QString projectId = currentProjectId();
    const Project *project = findProject(projectId);
    if (project == nullptr) {
        return;
    }

    QString errorMessage;
    if (!m_repository.runIntakeValidation(projectId, &errorMessage)) {
        QMessageBox::critical(this, QStringLiteral("Could not run intake check"), errorMessage);
        return;
    }

    refreshProjects();
    selectProject(projectId);
    statusBar()->showMessage(QStringLiteral("Intake check finished for %1").arg(project->displayTitle()), 4000);
}

void MainWindow::deleteSelectedProject()
{
    const QString projectId = currentProjectId();
    const Project *project = findProject(projectId);
    if (project == nullptr) {
        return;
    }

    const QString projectTitle = project->displayTitle();

    const auto choice = QMessageBox::question(
        this,
        QStringLiteral("Delete project"),
        QStringLiteral("Delete %1? This removes it from local storage.").arg(projectTitle));

    if (choice != QMessageBox::Yes) {
        return;
    }

    QString errorMessage;
    if (!m_repository.remove(projectId, &errorMessage)) {
        QMessageBox::critical(this, QStringLiteral("Could not delete project"), errorMessage);
        return;
    }

    refreshProjects();
    statusBar()->showMessage(QStringLiteral("Deleted %1").arg(projectTitle), 4000);
}

void MainWindow::refreshProjects()
{
    const QString selectedProjectId = currentProjectId();

    QString errorMessage;
    m_projects = m_repository.all(&errorMessage);
    if (!errorMessage.isEmpty()) {
        QMessageBox::critical(this, QStringLiteral("Could not load projects"), errorMessage);
    }

    populateProjectList(selectedProjectId);
    updateStatusBar();
}

void MainWindow::handleSelectionChanged()
{
    const QString projectId = currentProjectId();
    const Project *project = findProject(projectId);
    if (project == nullptr) {
        showEmptyState();
        return;
    }

    showProject(*project);
}

void MainWindow::populateProjectList(const QString &selectedProjectId)
{
    m_projectList->clear();

    for (const Project &project : m_projects) {
        auto *item = new QListWidgetItem(project.displayTitle(), m_projectList);
        item->setData(Qt::UserRole, project.id);
        item->setToolTip(project.guideIntent + QStringLiteral("\nState: ") + project.stateLabel() + QStringLiteral("\nUnresolved: ") + project.unresolvedSummary());
        item->setText(project.displayTitle() + QStringLiteral("\n") + project.stateLabel().toUpper() + QStringLiteral(" • ") + project.summaryLine());
    }

    if (m_projects.isEmpty()) {
        showEmptyState();
        return;
    }

    QString projectToSelect = selectedProjectId;
    if (projectToSelect.isEmpty()) {
        projectToSelect = m_projects.front().id;
    }

    selectProject(projectToSelect);
}

void MainWindow::showEmptyState()
{
    m_detailStack->setCurrentWidget(m_emptyPage);
    m_editButton->setEnabled(false);
    m_blueprintButton->setEnabled(false);
    m_previewButton->setEnabled(false);
    m_validateButton->setEnabled(false);
    m_deleteButton->setEnabled(false);
    m_lensTitleValue->setText(QStringLiteral("No project selected"));
    m_lensStateValue->clear();
    m_lensRunStyleValue->clear();
    m_lensSpoilersValue->clear();
    m_lensDepthValue->clear();
    m_lensUnresolvedValue->clear();
    m_lensValidationStateValue->clear();
    m_lensValidationSummaryValue->clear();
    m_lensThemeCssValue->clear();
    m_lensBlueprintValue->clear();
    m_lensPreviewValue->clear();
    m_lensPreviewApprovalValue->clear();
    m_storagePathValue->setText(m_repository.projectsRootPath());
}

void MainWindow::showProject(const Project &project)
{
    QString repositoryError;
    const QVector<EditorialBlueprint> blueprints = m_repository.listBlueprints(project.id, &repositoryError);
    const QVector<PreviewPackage> previews = repositoryError.isEmpty()
        ? m_repository.listPreviews(project.id, &repositoryError)
        : QVector<PreviewPackage> {};
    const QVector<BuildJob> jobs = repositoryError.isEmpty()
        ? m_repository.listJobs(project.id, &repositoryError)
        : QVector<BuildJob> {};
    const QVector<ValidationReport> reports = repositoryError.isEmpty()
        ? m_repository.listValidationReports(project.id, &repositoryError)
        : QVector<ValidationReport> {};
    const EditorialBlueprint *latestBlueprint = blueprints.isEmpty() ? nullptr : &blueprints.front();
    const PreviewPackage *latestPreview = previews.isEmpty() ? nullptr : &previews.front();

    QString validationState = QStringLiteral("NOT RUN");
    QString validationSummary = QStringLiteral("No intake validation has been recorded yet.");
    QString lastChecked = QStringLiteral("Not yet");

    if (!repositoryError.isEmpty()) {
        validationState = QStringLiteral("ERROR");
        validationSummary = repositoryError;
        lastChecked = QStringLiteral("Unavailable");
    } else if (!jobs.isEmpty() || !reports.isEmpty()) {
        if (!jobs.isEmpty()) {
            validationState = jobs.front().status.toUpper();
            lastChecked = jobs.front().endedAt.isEmpty() ? jobs.front().startedAt : jobs.front().endedAt;
        }
        if (!reports.isEmpty()) {
            validationSummary = reports.front().summary;
            if (lastChecked == QStringLiteral("Not yet")) {
                lastChecked = reports.front().createdAt;
            }
        }
    }

    m_detailTitle->setText(project.displayTitle());
    m_detailSummary->setText(project.stateLabel().toUpper() + QStringLiteral(" • ") + project.summaryLine());
    m_stateValue->setText(project.stateLabel().toUpper());
    m_platformValue->setText(project.platform.isEmpty() ? QStringLiteral("Unresolved") : project.platform);
    m_guideIntentValue->setText(project.guideIntent);
    m_playerGoalValue->setText(project.playerGoal.isEmpty() ? QStringLiteral("Unresolved") : project.playerGoal);
    m_runStyleValue->setText(project.runStyle);
    m_spoilerPolicyValue->setText(project.spoilerPolicy);
    m_depthPreferenceValue->setText(project.depthPreference);
    m_unresolvedValue->setText(project.unresolvedFields().isEmpty() ? QStringLiteral("None") : project.unresolvedFields().join(QStringLiteral(", ")));
    m_validationStatusValue->setText(validationState);
    m_validationSummaryValue->setText(validationSummary);
    m_lastCheckedValue->setText(lastChecked);
    m_themeCssValue->setText(project.sharedThemeSummary());
    m_blueprintSummaryValue->setText(latestBlueprint == nullptr ? QStringLiteral("No editorial blueprint drafted yet.") : latestBlueprint->summaryLine());
    m_blueprintChaptersValue->setText(latestBlueprint == nullptr ? QStringLiteral("No chapter plans yet.") : latestBlueprint->chapterTitleSummary());
    m_blueprintUpdatedAtValue->setText(latestBlueprint == nullptr ? QStringLiteral("Not yet") : latestBlueprint->updatedAt);
    m_previewSummaryValue->setText(describePreview(latestPreview, latestBlueprint));
    m_previewApprovalValue->setText(latestPreview == nullptr ? QStringLiteral("NOT GENERATED") : latestPreview->approvalState.toUpper());
    m_previewSectionsValue->setText(latestPreview == nullptr ? QStringLiteral("No preview sections yet.") : latestPreview->sampleTitleSummary());
    m_previewUpdatedAtValue->setText(latestPreview == nullptr ? QStringLiteral("Not yet") : latestPreview->updatedAt);
    m_createdAtValue->setText(project.createdAt);
    m_identifierValue->setText(project.id);

    m_lensTitleValue->setText(project.displayTitle());
    m_lensStateValue->setText(project.stateLabel().toUpper());
    m_lensRunStyleValue->setText(project.runStyle);
    m_lensSpoilersValue->setText(project.spoilerPolicy);
    m_lensDepthValue->setText(project.depthPreference);
    m_lensUnresolvedValue->setText(project.unresolvedFields().isEmpty() ? QStringLiteral("None") : project.unresolvedFields().join(QStringLiteral(", ")));
    m_lensValidationStateValue->setText(validationState);
    m_lensValidationSummaryValue->setText(validationSummary);
    m_lensThemeCssValue->setText(project.sharedThemeSummary());
    m_lensBlueprintValue->setText(latestBlueprint == nullptr ? QStringLiteral("No blueprint") : latestBlueprint->summaryLine());
    m_lensPreviewValue->setText(describePreview(latestPreview, latestBlueprint));
    m_lensPreviewApprovalValue->setText(latestPreview == nullptr ? QStringLiteral("NOT GENERATED") : latestPreview->approvalState.toUpper());
    m_storagePathValue->setText(m_repository.projectRootPath(project.id));

    m_editButton->setEnabled(true);
    m_blueprintButton->setEnabled(true);
    m_previewButton->setEnabled(latestBlueprint != nullptr);
    m_validateButton->setEnabled(true);
    m_deleteButton->setEnabled(true);
    m_detailStack->setCurrentWidget(m_detailPage);
}

void MainWindow::selectProject(const QString &projectId)
{
    for (int row = 0; row < m_projectList->count(); ++row) {
        QListWidgetItem *item = m_projectList->item(row);
        if (item != nullptr && item->data(Qt::UserRole).toString() == projectId) {
            m_projectList->setCurrentItem(item);
            return;
        }
    }

    if (m_projectList->count() > 0) {
        m_projectList->setCurrentRow(0);
    } else {
        showEmptyState();
    }
}

QString MainWindow::currentProjectId() const
{
    const QListWidgetItem *item = m_projectList->currentItem();
    if (item == nullptr) {
        return {};
    }

    return item->data(Qt::UserRole).toString();
}

const Project *MainWindow::findProject(const QString &projectId) const
{
    for (const Project &project : m_projects) {
        if (project.id == projectId) {
            return &project;
        }
    }

    return nullptr;
}

void MainWindow::updateStatusBar()
{
    statusBar()->showMessage(QStringLiteral("%1 project(s) loaded • %2")
                                 .arg(m_projects.size())
                                 .arg(m_repository.projectsRootPath()));
}

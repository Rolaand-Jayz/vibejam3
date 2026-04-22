#include "new_project_dialog.hpp"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>

NewProjectDialog::NewProjectDialog(QWidget *parent)
    : QDialog(parent)
    , m_introLabel(new QLabel(this))
    , m_hintLabel(new QLabel(this))
    , m_submitButton(nullptr)
    , m_gameTitleEdit(new QLineEdit(this))
    , m_platformEdit(new QLineEdit(this))
    , m_guideIntentEdit(new QLineEdit(this))
    , m_playerGoalEdit(new QLineEdit(this))
    , m_runStyleCombo(new QComboBox(this))
    , m_spoilerPolicyCombo(new QComboBox(this))
    , m_depthPreferenceCombo(new QComboBox(this))
    , m_sharedThemeCssEdit(new QPlainTextEdit(this))
{
    setModal(true);
    resize(680, 620);

    auto *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(20, 20, 20, 20);
    rootLayout->setSpacing(16);

    m_introLabel->setWordWrap(true);
    rootLayout->addWidget(m_introLabel);

    m_hintLabel->setWordWrap(true);
    m_hintLabel->setObjectName(QStringLiteral("heroSubtitle"));
    rootLayout->addWidget(m_hintLabel);

    auto *formLayout = new QFormLayout();
    formLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    formLayout->setFormAlignment(Qt::AlignTop);
    formLayout->setHorizontalSpacing(16);
    formLayout->setVerticalSpacing(12);

    m_gameTitleEdit->setPlaceholderText(QStringLiteral("e.g. Final Fantasy X-2"));
    m_platformEdit->setPlaceholderText(QStringLiteral("e.g. PS2, Switch, PC"));
    m_guideIntentEdit->setText(QStringLiteral("complete walkthrough"));
    m_playerGoalEdit->setText(QStringLiteral("beat the main story"));
    m_sharedThemeCssEdit->setPlaceholderText(QStringLiteral(
        ".guide-theme-surface {\n"
        "  --app-bg: #171717;\n"
        "  --panel-bg: rgba(35, 24, 18, 0.92);\n"
        "  --accent: #f0b35a;\n"
        "}\n"));
    m_sharedThemeCssEdit->setMinimumHeight(180);

    m_runStyleCombo->addItems({
        QStringLiteral("casual"),
        QStringLiteral("optimized"),
        QStringLiteral("challenge"),
        QStringLiteral("speedrun"),
    });

    m_spoilerPolicyCombo->addItems({
        QStringLiteral("none"),
        QStringLiteral("minimal"),
        QStringLiteral("moderate"),
        QStringLiteral("full"),
    });
    m_spoilerPolicyCombo->setCurrentText(QStringLiteral("minimal"));

    m_depthPreferenceCombo->addItems({
        QStringLiteral("essential"),
        QStringLiteral("standard"),
        QStringLiteral("comprehensive"),
        QStringLiteral("completionist"),
        QStringLiteral("reference"),
    });
    m_depthPreferenceCombo->setCurrentText(QStringLiteral("standard"));

    formLayout->addRow(QStringLiteral("Game title *"), m_gameTitleEdit);
    formLayout->addRow(QStringLiteral("Platform"), m_platformEdit);
    formLayout->addRow(QStringLiteral("Guide intent *"), m_guideIntentEdit);
    formLayout->addRow(QStringLiteral("Player goal"), m_playerGoalEdit);
    formLayout->addRow(QStringLiteral("Run style"), m_runStyleCombo);
    formLayout->addRow(QStringLiteral("Spoiler policy"), m_spoilerPolicyCombo);
    formLayout->addRow(QStringLiteral("Depth preference"), m_depthPreferenceCombo);
    formLayout->addRow(QStringLiteral("Shared web / guide CSS"), m_sharedThemeCssEdit);
    rootLayout->addLayout(formLayout);

    auto *buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Ok, this);
    if (QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok)) {
        m_submitButton = okButton;
        m_submitButton->setObjectName(QStringLiteral("primaryButton"));
    }

    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &NewProjectDialog::validateAndAccept);
    rootLayout->addWidget(buttonBox);

    setDialogMode(
        QStringLiteral("New Guide Project"),
        QStringLiteral("Capture the core shape of a guide before the playthrough work begins."),
        QStringLiteral("Create project"));
}

ProjectDraft NewProjectDialog::draft() const
{
    return ProjectDraft {
        .gameTitle = m_gameTitleEdit->text(),
        .platform = m_platformEdit->text(),
        .guideIntent = m_guideIntentEdit->text(),
        .playerGoal = m_playerGoalEdit->text(),
        .runStyle = m_runStyleCombo->currentText(),
        .spoilerPolicy = m_spoilerPolicyCombo->currentText(),
        .depthPreference = m_depthPreferenceCombo->currentText(),
        .sharedThemeCss = m_sharedThemeCssEdit->toPlainText(),
    };
}

void NewProjectDialog::setDraft(const ProjectDraft &draft)
{
    m_gameTitleEdit->setText(draft.gameTitle);
    m_platformEdit->setText(draft.platform);
    m_guideIntentEdit->setText(draft.guideIntent);
    m_playerGoalEdit->setText(draft.playerGoal);
    m_runStyleCombo->setCurrentText(draft.runStyle.isEmpty() ? QStringLiteral("casual") : draft.runStyle);
    m_spoilerPolicyCombo->setCurrentText(draft.spoilerPolicy.isEmpty() ? QStringLiteral("minimal") : draft.spoilerPolicy);
    m_depthPreferenceCombo->setCurrentText(draft.depthPreference.isEmpty() ? QStringLiteral("standard") : draft.depthPreference);
    m_sharedThemeCssEdit->setPlainText(draft.sharedThemeCss);
}

void NewProjectDialog::setDialogMode(const QString &windowTitle, const QString &introText, const QString &submitText)
{
    setWindowTitle(windowTitle);
    m_introLabel->setText(introText);
    m_hintLabel->setText(QStringLiteral("Required fields are marked with *. Blank optional fields remain unresolved until you confirm them later. Shared CSS is optional and lets the web workspace and future guide surfaces share the same theme."));
    if (m_submitButton != nullptr) {
        m_submitButton->setText(submitText);
    }
}

void NewProjectDialog::validateAndAccept()
{
    if (m_gameTitleEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("Missing title"), QStringLiteral("Game title is required."));
        m_gameTitleEdit->setFocus();
        return;
    }

    if (m_guideIntentEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("Missing guide intent"), QStringLiteral("Guide intent is required."));
        m_guideIntentEdit->setFocus();
        return;
    }

    accept();
}

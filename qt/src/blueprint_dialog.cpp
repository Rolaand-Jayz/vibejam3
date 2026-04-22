#include "blueprint_dialog.hpp"

#include <QDialogButtonBox>
#include <QLabel>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFormLayout>

#include <utility>

namespace {
QString formatChapterPlans(const QVector<BlueprintChapterPlan> &chapterPlans)
{
    QStringList lines;
    lines.reserve(chapterPlans.size());
    for (const BlueprintChapterPlan &chapterPlan : chapterPlans) {
        lines.push_back(QStringLiteral("%1 :: %2").arg(chapterPlan.title, chapterPlan.purpose));
    }
    return lines.join(QStringLiteral("\n"));
}

bool parseChapterPlans(const QString &rawText, QVector<BlueprintChapterPlan> *chapterPlans, QString *errorMessage)
{
    if (chapterPlans == nullptr) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("Internal error: missing chapter plan output.");
        }
        return false;
    }

    const QStringList rawLines = rawText.split('\n');
    QVector<BlueprintChapterPlan> parsedPlans;
    for (const QString &rawLine : rawLines) {
        const QString line = rawLine.trimmed();
        if (line.isEmpty()) {
            continue;
        }

        const int separatorIndex = line.indexOf(QStringLiteral("::"));
        if (separatorIndex == -1) {
            if (errorMessage != nullptr) {
                *errorMessage = QStringLiteral("Each chapter plan line must use “Chapter title :: purpose”.");
            }
            return false;
        }

        const QString title = line.left(separatorIndex).trimmed();
        const QString purpose = line.mid(separatorIndex + 2).trimmed();
        if (title.isEmpty() || purpose.isEmpty()) {
            if (errorMessage != nullptr) {
                *errorMessage = QStringLiteral("Each chapter plan line needs both a title and a purpose.");
            }
            return false;
        }

        parsedPlans.push_back(BlueprintChapterPlan { title, purpose });
    }

    if (parsedPlans.isEmpty()) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("Add at least one chapter plan before saving the blueprint.");
        }
        return false;
    }

    *chapterPlans = parsedPlans;
    if (errorMessage != nullptr) {
        errorMessage->clear();
    }
    return true;
}
}

BlueprintDialog::BlueprintDialog(QWidget *parent)
    : QDialog(parent)
    , m_introLabel(new QLabel(this))
    , m_hintLabel(new QLabel(this))
    , m_submitButton(nullptr)
    , m_chapterPlansEdit(new QPlainTextEdit(this))
    , m_terminologyRulesEdit(new QPlainTextEdit(this))
    , m_styleBibleEdit(new QPlainTextEdit(this))
    , m_crossReferencePlanEdit(new QPlainTextEdit(this))
    , m_checklistPlanEdit(new QPlainTextEdit(this))
    , m_visualPackIntentEdit(new QPlainTextEdit(this))
    , m_spoilerGuardrailsEdit(new QPlainTextEdit(this))
{
    setModal(true);
    resize(860, 860);

    auto *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(20, 20, 20, 20);
    rootLayout->setSpacing(16);

    m_introLabel->setWordWrap(true);
    rootLayout->addWidget(m_introLabel);

    m_hintLabel->setWordWrap(true);
    m_hintLabel->setObjectName(QStringLiteral("heroSubtitle"));
    rootLayout->addWidget(m_hintLabel);

    auto *formLayout = new QFormLayout();
    formLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignTop);
    formLayout->setFormAlignment(Qt::AlignTop);
    formLayout->setHorizontalSpacing(16);
    formLayout->setVerticalSpacing(12);

    m_chapterPlansEdit->setPlaceholderText(QStringLiteral(
        "Opening route :: Establish the first hours, required setup, and early warnings.\n"
        "Core progression :: Explain the main route, detours, and tactical checkpoints.\n"
        "Cleanup :: Capture missables, postgame cleanup, and ending requirements."));
    m_chapterPlansEdit->setMinimumHeight(180);

    const QList<QPlainTextEdit *> editors {
        m_terminologyRulesEdit,
        m_styleBibleEdit,
        m_crossReferencePlanEdit,
        m_checklistPlanEdit,
        m_visualPackIntentEdit,
        m_spoilerGuardrailsEdit,
    };
    for (QPlainTextEdit *editor : editors) {
        editor->setMinimumHeight(110);
    }

    formLayout->addRow(QStringLiteral("Chapter plans *"), m_chapterPlansEdit);
    formLayout->addRow(QStringLiteral("Terminology rules *"), m_terminologyRulesEdit);
    formLayout->addRow(QStringLiteral("Style bible *"), m_styleBibleEdit);
    formLayout->addRow(QStringLiteral("Cross-reference plan *"), m_crossReferencePlanEdit);
    formLayout->addRow(QStringLiteral("Checklist plan *"), m_checklistPlanEdit);
    formLayout->addRow(QStringLiteral("Visual pack intent *"), m_visualPackIntentEdit);
    formLayout->addRow(QStringLiteral("Spoiler guardrails *"), m_spoilerGuardrailsEdit);
    rootLayout->addLayout(formLayout);

    auto *buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Ok, this);
    if (QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok)) {
        m_submitButton = okButton;
        m_submitButton->setObjectName(QStringLiteral("primaryButton"));
    }

    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &BlueprintDialog::validateAndAccept);
    rootLayout->addWidget(buttonBox);

    setDialogMode(
        QStringLiteral("Draft Editorial Blueprint"),
        QStringLiteral("Define the real editorial structure that preview and full-build work will later follow."),
        QStringLiteral("Save blueprint"));
}

BlueprintDraft BlueprintDialog::draft() const
{
    QVector<BlueprintChapterPlan> chapterPlans;
    QString ignoredError;
    parseChapterPlans(m_chapterPlansEdit->toPlainText(), &chapterPlans, &ignoredError);

    return BlueprintDraft {
        .chapterPlans = chapterPlans,
        .terminologyRules = m_terminologyRulesEdit->toPlainText(),
        .styleBible = m_styleBibleEdit->toPlainText(),
        .crossReferencePlan = m_crossReferencePlanEdit->toPlainText(),
        .checklistPlan = m_checklistPlanEdit->toPlainText(),
        .visualPackIntent = m_visualPackIntentEdit->toPlainText(),
        .spoilerGuardrails = m_spoilerGuardrailsEdit->toPlainText(),
    };
}

void BlueprintDialog::setDraft(const BlueprintDraft &draft)
{
    m_chapterPlansEdit->setPlainText(formatChapterPlans(draft.chapterPlans));
    m_terminologyRulesEdit->setPlainText(draft.terminologyRules);
    m_styleBibleEdit->setPlainText(draft.styleBible);
    m_crossReferencePlanEdit->setPlainText(draft.crossReferencePlan);
    m_checklistPlanEdit->setPlainText(draft.checklistPlan);
    m_visualPackIntentEdit->setPlainText(draft.visualPackIntent);
    m_spoilerGuardrailsEdit->setPlainText(draft.spoilerGuardrails);
}

void BlueprintDialog::setDialogMode(const QString &windowTitle, const QString &introText, const QString &submitText)
{
    setWindowTitle(windowTitle);
    m_introLabel->setText(introText);
    m_hintLabel->setText(QStringLiteral("Use one line per chapter plan in the form “Chapter title :: purpose”. Save only what should count as the real editorial plan for this guide version."));
    if (m_submitButton != nullptr) {
        m_submitButton->setText(submitText);
    }
}

void BlueprintDialog::validateAndAccept()
{
    QVector<BlueprintChapterPlan> chapterPlans;
    QString errorMessage;
    if (!parseChapterPlans(m_chapterPlansEdit->toPlainText(), &chapterPlans, &errorMessage)) {
        QMessageBox::warning(this, QStringLiteral("Blueprint is incomplete"), errorMessage);
        m_chapterPlansEdit->setFocus();
        return;
    }

    const QList<std::pair<QString, QPlainTextEdit *>> requiredEditors {
        { QStringLiteral("Terminology rules are required."), m_terminologyRulesEdit },
        { QStringLiteral("Style bible is required."), m_styleBibleEdit },
        { QStringLiteral("Cross-reference plan is required."), m_crossReferencePlanEdit },
        { QStringLiteral("Checklist plan is required."), m_checklistPlanEdit },
        { QStringLiteral("Visual pack intent is required."), m_visualPackIntentEdit },
        { QStringLiteral("Spoiler guardrails are required."), m_spoilerGuardrailsEdit },
    };

    for (const auto &entry : requiredEditors) {
        if (entry.second->toPlainText().trimmed().isEmpty()) {
            QMessageBox::warning(this, QStringLiteral("Blueprint is incomplete"), entry.first);
            entry.second->setFocus();
            return;
        }
    }

    accept();
}

#include "preview_review_dialog.hpp"

#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>

namespace {
QString formatPreviewBody(const QString &projectTitle, const PreviewPackage &preview)
{
    QStringList blocks;
    blocks.push_back(QStringLiteral("%1 — Preview v%2").arg(projectTitle).arg(preview.version));
    blocks.push_back(QStringLiteral("Blueprint v%1 • %2").arg(preview.blueprintVersion).arg(preview.approvalState));

    for (const PreviewSampleSection &section : preview.sampleSections) {
        blocks.push_back(QStringLiteral("\n[%1] %2").arg(section.kind.toUpper(), section.title));
        blocks.push_back(section.summary);
        blocks.push_back(section.body);
    }

    return blocks.join(QStringLiteral("\n\n"));
}
}

PreviewReviewDialog::PreviewReviewDialog(QWidget *parent)
    : QDialog(parent)
    , m_introLabel(new QLabel(this))
    , m_summaryLabel(new QLabel(this))
    , m_previewBodyEdit(new QPlainTextEdit(this))
    , m_reviewNotesEdit(new QPlainTextEdit(this))
{
    setModal(true);
    resize(860, 780);

    auto *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(20, 20, 20, 20);
    rootLayout->setSpacing(16);

    m_introLabel->setWordWrap(true);
    rootLayout->addWidget(m_introLabel);

    m_summaryLabel->setWordWrap(true);
    m_summaryLabel->setObjectName(QStringLiteral("heroSubtitle"));
    rootLayout->addWidget(m_summaryLabel);

    auto *previewLabel = new QLabel(QStringLiteral("Preview content"), this);
    previewLabel->setObjectName(QStringLiteral("sectionTitle"));
    rootLayout->addWidget(previewLabel);

    m_previewBodyEdit->setReadOnly(true);
    m_previewBodyEdit->setMinimumHeight(360);
    rootLayout->addWidget(m_previewBodyEdit, 1);

    auto *reviewLabel = new QLabel(QStringLiteral("Review notes"), this);
    reviewLabel->setObjectName(QStringLiteral("sectionTitle"));
    rootLayout->addWidget(reviewLabel);

    m_reviewNotesEdit->setPlaceholderText(QStringLiteral("Record whether the tone, density, checklist behavior, and reference formatting are representative enough to continue later."));
    m_reviewNotesEdit->setMinimumHeight(140);
    rootLayout->addWidget(m_reviewNotesEdit);

    auto *buttonRow = new QHBoxLayout();
    buttonRow->setSpacing(10);

    auto *closeButton = new QPushButton(QStringLiteral("Close"), this);
    auto *requestChangesButton = new QPushButton(QStringLiteral("Request Changes"), this);
    auto *approveButton = new QPushButton(QStringLiteral("Approve Preview"), this);
    approveButton->setObjectName(QStringLiteral("primaryButton"));

    buttonRow->addWidget(closeButton);
    buttonRow->addStretch();
    buttonRow->addWidget(requestChangesButton);
    buttonRow->addWidget(approveButton);
    rootLayout->addLayout(buttonRow);

    connect(closeButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(requestChangesButton, &QPushButton::clicked, this, &PreviewReviewDialog::requestChanges);
    connect(approveButton, &QPushButton::clicked, this, &PreviewReviewDialog::approvePreview);

    setDialogMode(
        QStringLiteral("Review Preview Package"),
        QStringLiteral("Inspect the durable preview output and record an explicit approval decision before later full-build work continues."));
}

void PreviewReviewDialog::setDialogMode(const QString &windowTitle, const QString &introText)
{
    setWindowTitle(windowTitle);
    m_introLabel->setText(introText);
}

void PreviewReviewDialog::setPreview(const QString &projectTitle, const PreviewPackage &preview)
{
    m_approvalState.clear();
    m_summaryLabel->setText(QStringLiteral("%1 • %2 • Generated %3").arg(projectTitle, preview.summaryLine(), preview.generatedAt));
    m_previewBodyEdit->setPlainText(formatPreviewBody(projectTitle, preview));
    m_reviewNotesEdit->setPlainText(preview.reviewNotes);
}

QString PreviewReviewDialog::approvalState() const
{
    return m_approvalState;
}

QString PreviewReviewDialog::reviewNotes() const
{
    return m_reviewNotesEdit->toPlainText();
}

void PreviewReviewDialog::approvePreview()
{
    m_approvalState = QStringLiteral("approved");
    accept();
}

void PreviewReviewDialog::requestChanges()
{
    if (m_reviewNotesEdit->toPlainText().trimmed().isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("Review notes required"), QStringLiteral("Explain what should change before marking this preview for revision."));
        m_reviewNotesEdit->setFocus();
        return;
    }

    m_approvalState = QStringLiteral("changes-requested");
    accept();
}

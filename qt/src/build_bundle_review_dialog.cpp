#include "build_bundle_review_dialog.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>

namespace {
QString formatBuildBundleBody(const QString &projectTitle, const GuideBuildBundle &bundle, const ValidationReport &report)
{
    QStringList blocks;
    blocks.push_back(QStringLiteral("%1 — Full build bundle v%2").arg(projectTitle).arg(bundle.version));
    blocks.push_back(QStringLiteral("Blueprint v%1 • %2").arg(bundle.blueprintVersion).arg(bundle.validationState));
    blocks.push_back(QStringLiteral("Validation summary: %1").arg(report.summary));
    blocks.push_back(QStringLiteral("Blocking findings: %1 • Warning findings: %2").arg(report.blockingCount).arg(report.warningCount));

    for (const GuideBuildUnit &unit : bundle.units) {
        blocks.push_back(QStringLiteral("\n[%1] %2").arg(unit.kind.toUpper(), unit.title));
        blocks.push_back(unit.purpose);
        blocks.push_back(unit.body);
    }

    return blocks.join(QStringLiteral("\n\n"));
}
}

BuildBundleReviewDialog::BuildBundleReviewDialog(QWidget *parent)
    : QDialog(parent)
    , m_introLabel(new QLabel(this))
    , m_summaryLabel(new QLabel(this))
    , m_bundleBodyEdit(new QPlainTextEdit(this))
{
    setModal(true);
    resize(920, 800);

    auto *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(20, 20, 20, 20);
    rootLayout->setSpacing(16);

    m_introLabel->setWordWrap(true);
    rootLayout->addWidget(m_introLabel);

    m_summaryLabel->setWordWrap(true);
    m_summaryLabel->setObjectName(QStringLiteral("heroSubtitle"));
    rootLayout->addWidget(m_summaryLabel);

    auto *bundleLabel = new QLabel(QStringLiteral("Build bundle content"), this);
    bundleLabel->setObjectName(QStringLiteral("sectionTitle"));
    rootLayout->addWidget(bundleLabel);

    m_bundleBodyEdit->setReadOnly(true);
    m_bundleBodyEdit->setMinimumHeight(520);
    rootLayout->addWidget(m_bundleBodyEdit, 1);

    auto *buttonRow = new QHBoxLayout();
    buttonRow->setSpacing(10);
    auto *closeButton = new QPushButton(QStringLiteral("Close"), this);
    closeButton->setObjectName(QStringLiteral("primaryButton"));
    buttonRow->addStretch();
    buttonRow->addWidget(closeButton);
    rootLayout->addLayout(buttonRow);

    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);

    setDialogMode(
        QStringLiteral("Full Build Bundle"),
        QStringLiteral("Inspect the durable full-build bundle and its validation outcome before any later freeze step is allowed."));
}

void BuildBundleReviewDialog::setDialogMode(const QString &windowTitle, const QString &introText)
{
    setWindowTitle(windowTitle);
    m_introLabel->setText(introText);
}

void BuildBundleReviewDialog::setBuildBundle(const QString &projectTitle, const GuideBuildBundle &bundle, const ValidationReport &report)
{
    m_summaryLabel->setText(QStringLiteral("%1 • %2 • Generated %3").arg(projectTitle, bundle.summaryLine(), bundle.generatedAt));
    m_bundleBodyEdit->setPlainText(formatBuildBundleBody(projectTitle, bundle, report));
}

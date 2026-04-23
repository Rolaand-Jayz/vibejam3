#include "knowledge_workbench_dialog.hpp"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>

namespace {
QString formatList(const QStringList &values)
{
    return values.join(QStringLiteral("\n"));
}

QStringList parseList(const QString &rawText)
{
    const QString normalized = rawText;
    const QStringList rawParts = normalized.split(QRegularExpression(QStringLiteral("[\n,]")), Qt::SkipEmptyParts);
    QStringList parsed;
    for (const QString &part : rawParts) {
        const QString trimmed = part.trimmed();
        if (!trimmed.isEmpty() && !parsed.contains(trimmed)) {
            parsed.push_back(trimmed);
        }
    }
    return parsed;
}

QString formatAttributes(const QVector<KnowledgeAttribute> &attributes)
{
    QStringList lines;
    lines.reserve(attributes.size());
    for (const KnowledgeAttribute &attribute : attributes) {
        lines.push_back(QStringLiteral("%1 :: %2").arg(attribute.key, attribute.value));
    }
    return lines.join(QStringLiteral("\n"));
}

bool parseAttributes(const QString &rawText, QVector<KnowledgeAttribute> *attributes, QString *errorMessage)
{
    if (attributes == nullptr) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("Internal error: missing knowledge attribute output.");
        }
        return false;
    }

    const QStringList rawLines = rawText.split('\n');
    QVector<KnowledgeAttribute> parsed;
    for (const QString &rawLine : rawLines) {
        const QString line = rawLine.trimmed();
        if (line.isEmpty()) {
            continue;
        }

        const int separatorIndex = line.indexOf(QStringLiteral("::"));
        if (separatorIndex == -1) {
            if (errorMessage != nullptr) {
                *errorMessage = QStringLiteral("Each knowledge attribute line must use “Key :: Value”.");
            }
            return false;
        }

        const QString key = line.left(separatorIndex).trimmed();
        const QString value = line.mid(separatorIndex + 2).trimmed();
        if (key.isEmpty() || value.isEmpty()) {
            if (errorMessage != nullptr) {
                *errorMessage = QStringLiteral("Each knowledge attribute line needs both a key and a value.");
            }
            return false;
        }

        parsed.push_back(KnowledgeAttribute { key, value });
    }

    *attributes = parsed;
    if (errorMessage != nullptr) {
        errorMessage->clear();
    }
    return true;
}
}

KnowledgeWorkbenchDialog::KnowledgeWorkbenchDialog(QWidget *parent)
    : QDialog(parent)
    , m_introLabel(new QLabel(this))
    , m_hintLabel(new QLabel(this))
    , m_submitButton(nullptr)
    , m_newEntryButton(nullptr)
    , m_entryList(new QListWidget(this))
    , m_canonicalNameEdit(new QLineEdit(this))
    , m_categoryCombo(new QComboBox(this))
    , m_visibilityCombo(new QComboBox(this))
    , m_scopeCombo(new QComboBox(this))
    , m_confidenceCombo(new QComboBox(this))
    , m_aliasesEdit(new QPlainTextEdit(this))
    , m_summaryEdit(new QPlainTextEdit(this))
    , m_attributesEdit(new QPlainTextEdit(this))
    , m_sourceTypeEdit(new QLineEdit(this))
    , m_sourceTitleEdit(new QLineEdit(this))
    , m_sourceUriEdit(new QLineEdit(this))
    , m_sourceTrustCombo(new QComboBox(this))
    , m_sourceExcerptEdit(new QPlainTextEdit(this))
    , m_conflictSummaryEdit(new QPlainTextEdit(this))
    , m_conflictTypeEdit(new QLineEdit(this))
    , m_conflictSeverityCombo(new QComboBox(this))
    , m_conflictResolutionCombo(new QComboBox(this))
    , m_recommendedHandlingEdit(new QPlainTextEdit(this))
    , m_versionTagsEdit(new QPlainTextEdit(this))
{
    setModal(true);
    resize(1160, 920);

    auto *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(20, 20, 20, 20);
    rootLayout->setSpacing(16);

    m_introLabel->setWordWrap(true);
    rootLayout->addWidget(m_introLabel);

    m_hintLabel->setWordWrap(true);
    m_hintLabel->setObjectName(QStringLiteral("heroSubtitle"));
    rootLayout->addWidget(m_hintLabel);

    auto *bodyLayout = new QHBoxLayout();
    bodyLayout->setSpacing(16);

    auto *listColumn = new QVBoxLayout();
    listColumn->setSpacing(12);
    auto *listTitle = new QLabel(QStringLiteral("Stored entries"), this);
    listTitle->setObjectName(QStringLiteral("sectionTitle"));
    m_entryList->setMinimumWidth(280);
    m_entryList->setSelectionMode(QAbstractItemView::SingleSelection);
    m_newEntryButton = new QPushButton(QStringLiteral("New entry"), this);
    listColumn->addWidget(listTitle);
    listColumn->addWidget(m_entryList, 1);
    listColumn->addWidget(m_newEntryButton);
    bodyLayout->addLayout(listColumn, 0);

    auto *formColumn = new QVBoxLayout();
    formColumn->setSpacing(14);

    auto *identityForm = new QFormLayout();
    identityForm->setLabelAlignment(Qt::AlignLeft | Qt::AlignTop);
    identityForm->setFormAlignment(Qt::AlignTop);
    identityForm->setHorizontalSpacing(16);
    identityForm->setVerticalSpacing(12);

    m_categoryCombo->addItems({
        QStringLiteral("route-note"),
        QStringLiteral("quest"),
        QStringLiteral("system"),
        QStringLiteral("item"),
        QStringLiteral("location"),
        QStringLiteral("enemy"),
        QStringLiteral("skill"),
        QStringLiteral("class"),
        QStringLiteral("vendor"),
        QStringLiteral("map-region"),
    });
    m_visibilityCombo->addItems({ QStringLiteral("visible-guide"), QStringLiteral("hidden-codex") });
    m_scopeCombo->addItems({ QStringLiteral("broad"), QStringLiteral("run-specific") });
    m_confidenceCombo->addItems({ QStringLiteral("low"), QStringLiteral("medium"), QStringLiteral("high") });
    m_sourceTrustCombo->addItems({
        QStringLiteral("primary"),
        QStringLiteral("secondary"),
        QStringLiteral("community-verified"),
        QStringLiteral("working-notes"),
        QStringLiteral("uncertain"),
    });
    m_conflictSeverityCombo->addItems({ QStringLiteral("warning"), QStringLiteral("blocking") });
    m_conflictResolutionCombo->addItems({ QStringLiteral("open"), QStringLiteral("monitoring"), QStringLiteral("resolved") });

    const QList<QPlainTextEdit *> tallerEditors {
        m_aliasesEdit,
        m_summaryEdit,
        m_attributesEdit,
        m_sourceExcerptEdit,
        m_conflictSummaryEdit,
        m_recommendedHandlingEdit,
        m_versionTagsEdit,
    };
    for (QPlainTextEdit *editor : tallerEditors) {
        editor->setMinimumHeight(90);
    }
    m_summaryEdit->setMinimumHeight(120);
    m_sourceExcerptEdit->setMinimumHeight(120);
    m_recommendedHandlingEdit->setMinimumHeight(120);

    identityForm->addRow(QStringLiteral("Canonical name *"), m_canonicalNameEdit);
    identityForm->addRow(QStringLiteral("Category"), m_categoryCombo);
    identityForm->addRow(QStringLiteral("Visibility"), m_visibilityCombo);
    identityForm->addRow(QStringLiteral("Scope"), m_scopeCombo);
    identityForm->addRow(QStringLiteral("Confidence"), m_confidenceCombo);
    identityForm->addRow(QStringLiteral("Aliases"), m_aliasesEdit);
    identityForm->addRow(QStringLiteral("Summary *"), m_summaryEdit);
    identityForm->addRow(QStringLiteral("Structured attributes"), m_attributesEdit);
    identityForm->addRow(QStringLiteral("Version tags"), m_versionTagsEdit);
    identityForm->addRow(QStringLiteral("Source type *"), m_sourceTypeEdit);
    identityForm->addRow(QStringLiteral("Source title *"), m_sourceTitleEdit);
    identityForm->addRow(QStringLiteral("Source URI"), m_sourceUriEdit);
    identityForm->addRow(QStringLiteral("Source trust"), m_sourceTrustCombo);
    identityForm->addRow(QStringLiteral("Source excerpt *"), m_sourceExcerptEdit);
    identityForm->addRow(QStringLiteral("Conflict summary"), m_conflictSummaryEdit);
    identityForm->addRow(QStringLiteral("Conflict type"), m_conflictTypeEdit);
    identityForm->addRow(QStringLiteral("Conflict severity"), m_conflictSeverityCombo);
    identityForm->addRow(QStringLiteral("Conflict status"), m_conflictResolutionCombo);
    identityForm->addRow(QStringLiteral("Recommended handling"), m_recommendedHandlingEdit);
    formColumn->addLayout(identityForm, 1);
    bodyLayout->addLayout(formColumn, 1);

    rootLayout->addLayout(bodyLayout, 1);

    auto *buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Ok, this);
    if (QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok)) {
        m_submitButton = okButton;
        m_submitButton->setObjectName(QStringLiteral("primaryButton"));
    }

    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &KnowledgeWorkbenchDialog::validateAndAccept);
    connect(m_entryList, &QListWidget::itemSelectionChanged, this, &KnowledgeWorkbenchDialog::handleSelectionChanged);
    connect(m_newEntryButton, &QPushButton::clicked, this, &KnowledgeWorkbenchDialog::resetForNewEntry);
    rootLayout->addWidget(buttonBox);

    setDialogMode(
        QStringLiteral("Knowledge Registry"),
        QStringLiteral("Store normalized facts, provenance, and hidden codex notes before they become guide prose."),
        QStringLiteral("Save knowledge"));
}

KnowledgeEntityDraft KnowledgeWorkbenchDialog::draft() const
{
    QVector<KnowledgeAttribute> attributes;
    QString ignoredError;
    parseAttributes(m_attributesEdit->toPlainText(), &attributes, &ignoredError);

    QVector<SourceReference> sourceRefs;
    if (!m_sourceTypeEdit->text().trimmed().isEmpty() || !m_sourceTitleEdit->text().trimmed().isEmpty() || !m_sourceExcerptEdit->toPlainText().trimmed().isEmpty()) {
        sourceRefs.push_back(SourceReference {
            .id = QString(),
            .sourceType = m_sourceTypeEdit->text().trimmed(),
            .sourceTitle = m_sourceTitleEdit->text().trimmed(),
            .sourceUri = m_sourceUriEdit->text().trimmed(),
            .excerpt = m_sourceExcerptEdit->toPlainText().trimmed(),
            .retrievalDate = QString(),
            .trustClassification = m_sourceTrustCombo->currentText(),
        });
    }

    QVector<ConflictRecord> conflicts;
    if (!m_conflictSummaryEdit->toPlainText().trimmed().isEmpty()) {
        conflicts.push_back(ConflictRecord {
            .id = QString(),
            .conflictSummary = m_conflictSummaryEdit->toPlainText().trimmed(),
            .conflictType = m_conflictTypeEdit->text().trimmed(),
            .severity = m_conflictSeverityCombo->currentText(),
            .resolutionStatus = m_conflictResolutionCombo->currentText(),
            .recommendedHandling = m_recommendedHandlingEdit->toPlainText().trimmed(),
        });
    }

    return KnowledgeEntityDraft {
        .canonicalName = m_canonicalNameEdit->text().trimmed(),
        .category = m_categoryCombo->currentText(),
        .visibility = m_visibilityCombo->currentText(),
        .scope = m_scopeCombo->currentText(),
        .confidence = m_confidenceCombo->currentText(),
        .aliases = parseList(m_aliasesEdit->toPlainText()),
        .summary = m_summaryEdit->toPlainText().trimmed(),
        .structuredAttributes = attributes,
        .sourceRefs = sourceRefs,
        .conflictMarkers = conflicts,
        .versionTags = parseList(m_versionTagsEdit->toPlainText()),
    };
}

QString KnowledgeWorkbenchDialog::selectedKnowledgeId() const
{
    return m_selectedKnowledgeId;
}

void KnowledgeWorkbenchDialog::setProjectContext(const QString &projectTitle, const QVector<KnowledgeEntity> &entities, const KnowledgeEntityDraft &defaultDraft)
{
    m_entities = entities;
    m_defaultDraft = defaultDraft;
    populateList();
    resetForNewEntry();
    m_hintLabel->setText(QStringLiteral("%1 keeps visible guide facts and hidden codex notes separate. Select an existing entry to edit it, or start a new one.").arg(projectTitle));
}

void KnowledgeWorkbenchDialog::setDialogMode(const QString &windowTitle, const QString &introText, const QString &submitText)
{
    setWindowTitle(windowTitle);
    m_introLabel->setText(introText);
    if (m_submitButton != nullptr) {
        m_submitButton->setText(submitText);
    }
}

void KnowledgeWorkbenchDialog::handleSelectionChanged()
{
    const QListWidgetItem *item = m_entryList->currentItem();
    if (item == nullptr) {
        return;
    }

    const QString knowledgeId = item->data(Qt::UserRole).toString();
    for (const KnowledgeEntity &entity : m_entities) {
        if (entity.id == knowledgeId) {
            applyDraft(entity.toDraft(), entity.id);
            return;
        }
    }
}

void KnowledgeWorkbenchDialog::resetForNewEntry()
{
    m_entryList->clearSelection();
    applyDraft(m_defaultDraft);
}

void KnowledgeWorkbenchDialog::validateAndAccept()
{
    if (m_canonicalNameEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("Knowledge entry is incomplete"), QStringLiteral("Canonical name is required."));
        m_canonicalNameEdit->setFocus();
        return;
    }

    if (m_summaryEdit->toPlainText().trimmed().isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("Knowledge entry is incomplete"), QStringLiteral("Summary is required."));
        m_summaryEdit->setFocus();
        return;
    }

    QVector<KnowledgeAttribute> attributes;
    QString errorMessage;
    if (!parseAttributes(m_attributesEdit->toPlainText(), &attributes, &errorMessage)) {
        QMessageBox::warning(this, QStringLiteral("Knowledge entry is incomplete"), errorMessage);
        m_attributesEdit->setFocus();
        return;
    }

    if (m_sourceTypeEdit->text().trimmed().isEmpty() || m_sourceTitleEdit->text().trimmed().isEmpty() || m_sourceExcerptEdit->toPlainText().trimmed().isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("Knowledge entry is incomplete"), QStringLiteral("Source type, source title, and source excerpt are required."));
        m_sourceTitleEdit->setFocus();
        return;
    }

    if (!m_conflictSummaryEdit->toPlainText().trimmed().isEmpty()
        && (m_conflictTypeEdit->text().trimmed().isEmpty() || m_recommendedHandlingEdit->toPlainText().trimmed().isEmpty())) {
        QMessageBox::warning(this, QStringLiteral("Knowledge entry is incomplete"), QStringLiteral("Conflict type and recommended handling are required when a conflict summary is provided."));
        m_conflictTypeEdit->setFocus();
        return;
    }

    accept();
}

void KnowledgeWorkbenchDialog::applyDraft(const KnowledgeEntityDraft &draft, const QString &knowledgeId)
{
    m_selectedKnowledgeId = knowledgeId;
    m_canonicalNameEdit->setText(draft.canonicalName);
    m_categoryCombo->setCurrentText(draft.category.isEmpty() ? QStringLiteral("route-note") : draft.category);
    m_visibilityCombo->setCurrentText(draft.visibility.isEmpty() ? QStringLiteral("visible-guide") : draft.visibility);
    m_scopeCombo->setCurrentText(draft.scope.isEmpty() ? QStringLiteral("broad") : draft.scope);
    m_confidenceCombo->setCurrentText(draft.confidence.isEmpty() ? QStringLiteral("medium") : draft.confidence);
    m_aliasesEdit->setPlainText(formatList(draft.aliases));
    m_summaryEdit->setPlainText(draft.summary);
    m_attributesEdit->setPlainText(formatAttributes(draft.structuredAttributes));
    m_versionTagsEdit->setPlainText(formatList(draft.versionTags));

    const SourceReference primarySource = draft.sourceRefs.isEmpty() ? SourceReference {} : draft.sourceRefs.front();
    m_sourceTypeEdit->setText(primarySource.sourceType.isEmpty() ? QStringLiteral("manual-entry") : primarySource.sourceType);
    m_sourceTitleEdit->setText(primarySource.sourceTitle);
    m_sourceUriEdit->setText(primarySource.sourceUri);
    m_sourceTrustCombo->setCurrentText(primarySource.trustClassification.isEmpty() ? QStringLiteral("working-notes") : primarySource.trustClassification);
    m_sourceExcerptEdit->setPlainText(primarySource.excerpt);

    const ConflictRecord primaryConflict = draft.conflictMarkers.isEmpty() ? ConflictRecord {} : draft.conflictMarkers.front();
    m_conflictSummaryEdit->setPlainText(primaryConflict.conflictSummary);
    m_conflictTypeEdit->setText(primaryConflict.conflictType);
    m_conflictSeverityCombo->setCurrentText(primaryConflict.severity.isEmpty() ? QStringLiteral("warning") : primaryConflict.severity);
    m_conflictResolutionCombo->setCurrentText(primaryConflict.resolutionStatus.isEmpty() ? QStringLiteral("open") : primaryConflict.resolutionStatus);
    m_recommendedHandlingEdit->setPlainText(primaryConflict.recommendedHandling);
}

void KnowledgeWorkbenchDialog::populateList()
{
    m_entryList->clear();
    for (const KnowledgeEntity &entity : m_entities) {
        auto *item = new QListWidgetItem(entity.canonicalName, m_entryList);
        item->setData(Qt::UserRole, entity.id);
        item->setToolTip(entity.summaryLine() + QStringLiteral("\n") + entity.summary);
        item->setText(entity.canonicalName + QStringLiteral("\n") + entity.visibilityLabel() + QStringLiteral(" • ") + entity.category);
    }
}

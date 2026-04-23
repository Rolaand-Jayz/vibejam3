#pragma once

#include "project.hpp"

#include <QDialog>

class QComboBox;
class QLabel;
class QLineEdit;
class QListWidget;
class QPlainTextEdit;
class QPushButton;

class KnowledgeWorkbenchDialog : public QDialog {
    Q_OBJECT

public:
    explicit KnowledgeWorkbenchDialog(QWidget *parent = nullptr);

    [[nodiscard]] KnowledgeEntityDraft draft() const;
    [[nodiscard]] QString selectedKnowledgeId() const;
    void setProjectContext(const QString &projectTitle, const QVector<KnowledgeEntity> &entities, const KnowledgeEntityDraft &defaultDraft);
    void setDialogMode(const QString &windowTitle, const QString &introText, const QString &submitText);

private slots:
    void handleSelectionChanged();
    void resetForNewEntry();
    void validateAndAccept();

private:
    void applyDraft(const KnowledgeEntityDraft &draft, const QString &knowledgeId = {});
    void populateList();

    QLabel *m_introLabel;
    QLabel *m_hintLabel;
    QPushButton *m_submitButton;
    QPushButton *m_newEntryButton;
    QListWidget *m_entryList;
    QLineEdit *m_canonicalNameEdit;
    QComboBox *m_categoryCombo;
    QComboBox *m_visibilityCombo;
    QComboBox *m_scopeCombo;
    QComboBox *m_confidenceCombo;
    QPlainTextEdit *m_aliasesEdit;
    QPlainTextEdit *m_summaryEdit;
    QPlainTextEdit *m_attributesEdit;
    QLineEdit *m_sourceTypeEdit;
    QLineEdit *m_sourceTitleEdit;
    QLineEdit *m_sourceUriEdit;
    QComboBox *m_sourceTrustCombo;
    QPlainTextEdit *m_sourceExcerptEdit;
    QPlainTextEdit *m_conflictSummaryEdit;
    QLineEdit *m_conflictTypeEdit;
    QComboBox *m_conflictSeverityCombo;
    QComboBox *m_conflictResolutionCombo;
    QPlainTextEdit *m_recommendedHandlingEdit;
    QPlainTextEdit *m_versionTagsEdit;

    QVector<KnowledgeEntity> m_entities;
    KnowledgeEntityDraft m_defaultDraft;
    QString m_selectedKnowledgeId;
};

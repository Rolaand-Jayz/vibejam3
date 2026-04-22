#pragma once

#include "project.hpp"

#include <QDialog>

class QLabel;
class QPlainTextEdit;
class QPushButton;

class BlueprintDialog : public QDialog {
    Q_OBJECT

public:
    explicit BlueprintDialog(QWidget *parent = nullptr);

    [[nodiscard]] BlueprintDraft draft() const;
    void setDraft(const BlueprintDraft &draft);
    void setDialogMode(const QString &windowTitle, const QString &introText, const QString &submitText);

private slots:
    void validateAndAccept();

private:
    QLabel *m_introLabel;
    QLabel *m_hintLabel;
    QPushButton *m_submitButton;
    QPlainTextEdit *m_chapterPlansEdit;
    QPlainTextEdit *m_terminologyRulesEdit;
    QPlainTextEdit *m_styleBibleEdit;
    QPlainTextEdit *m_crossReferencePlanEdit;
    QPlainTextEdit *m_checklistPlanEdit;
    QPlainTextEdit *m_visualPackIntentEdit;
    QPlainTextEdit *m_spoilerGuardrailsEdit;
};

#pragma once

#include "project.hpp"

#include <QDialog>

class QLabel;
class QPlainTextEdit;
class QPushButton;

class PreviewReviewDialog : public QDialog {
    Q_OBJECT

public:
    explicit PreviewReviewDialog(QWidget *parent = nullptr);

    void setDialogMode(const QString &windowTitle, const QString &introText);
    void setPreview(const QString &projectTitle, const PreviewPackage &preview);
    [[nodiscard]] QString approvalState() const;
    [[nodiscard]] QString reviewNotes() const;

private slots:
    void approvePreview();
    void requestChanges();

private:
    QLabel *m_introLabel;
    QLabel *m_summaryLabel;
    QPlainTextEdit *m_previewBodyEdit;
    QPlainTextEdit *m_reviewNotesEdit;
    QString m_approvalState;
};

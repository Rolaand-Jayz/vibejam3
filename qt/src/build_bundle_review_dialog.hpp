#pragma once

#include "project.hpp"

#include <QDialog>

class QLabel;
class QPlainTextEdit;

class BuildBundleReviewDialog : public QDialog {
    Q_OBJECT

public:
    explicit BuildBundleReviewDialog(QWidget *parent = nullptr);

    void setDialogMode(const QString &windowTitle, const QString &introText);
    void setBuildBundle(const QString &projectTitle, const GuideBuildBundle &bundle, const ValidationReport &report);

private:
    QLabel *m_introLabel;
    QLabel *m_summaryLabel;
    QPlainTextEdit *m_bundleBodyEdit;
};

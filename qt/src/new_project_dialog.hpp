#pragma once

#include "project.hpp"

#include <QDialog>

class QComboBox;
class QLabel;
class QLineEdit;
class QPlainTextEdit;
class QPushButton;

class NewProjectDialog : public QDialog {
    Q_OBJECT

public:
    explicit NewProjectDialog(QWidget *parent = nullptr);

    [[nodiscard]] ProjectDraft draft() const;
    void setDraft(const ProjectDraft &draft);
    void setDialogMode(const QString &windowTitle, const QString &introText, const QString &submitText);

private slots:
    void validateAndAccept();

private:
    QLabel *m_introLabel;
    QLabel *m_hintLabel;
    QPushButton *m_submitButton;
    QLineEdit *m_gameTitleEdit;
    QLineEdit *m_platformEdit;
    QLineEdit *m_guideIntentEdit;
    QLineEdit *m_playerGoalEdit;
    QComboBox *m_runStyleCombo;
    QComboBox *m_spoilerPolicyCombo;
    QComboBox *m_depthPreferenceCombo;
    QPlainTextEdit *m_sharedThemeCssEdit;
};

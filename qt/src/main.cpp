#include "main_window.hpp"

#include <QApplication>
#include <QCoreApplication>
#include <QFont>
#include <QPalette>
#include <QStyleFactory>

namespace {
void applyEditorialTheme(QApplication &app)
{
    app.setStyle(QStyleFactory::create(QStringLiteral("Fusion")));

    QPalette palette;
    palette.setColor(QPalette::Window, QColor(0x12, 0x15, 0x18));
    palette.setColor(QPalette::WindowText, QColor(0xEF, 0xF2, 0xF6));
    palette.setColor(QPalette::Base, QColor(0x0C, 0x0F, 0x12));
    palette.setColor(QPalette::AlternateBase, QColor(0x16, 0x1A, 0x1E));
    palette.setColor(QPalette::ToolTipBase, QColor(0x1B, 0x20, 0x25));
    palette.setColor(QPalette::ToolTipText, QColor(0xEF, 0xF2, 0xF6));
    palette.setColor(QPalette::Text, QColor(0xE5, 0xE8, 0xEB));
    palette.setColor(QPalette::Button, QColor(0x21, 0x26, 0x2C));
    palette.setColor(QPalette::ButtonText, QColor(0xF1, 0xF3, 0xF5));
    palette.setColor(QPalette::BrightText, QColor(0xFF, 0xF7, 0xED));
    palette.setColor(QPalette::Highlight, QColor(0x87, 0x67, 0x43));
    palette.setColor(QPalette::HighlightedText, QColor(0xF9, 0xF7, 0xF2));
    palette.setColor(QPalette::PlaceholderText, QColor(0x8D, 0x96, 0xA0));
    app.setPalette(palette);

    app.setStyleSheet(QStringLiteral(R"(
        QMainWindow {
            background: #0f1215;
        }
        QToolBar {
            background: #161a1e;
            border: none;
            border-bottom: 1px solid #2d343c;
            spacing: 8px;
            padding: 10px 14px;
        }
        QStatusBar {
            background: #15191d;
            border-top: 1px solid #2d343c;
            color: #b9c0c8;
        }
        QFrame#panelCard {
            background: #161a1e;
            border: 1px solid #2d343c;
            border-radius: 18px;
        }
        QLabel#heroTitle {
            color: #f4efe5;
            font-size: 26px;
            font-weight: 700;
            letter-spacing: 0.5px;
        }
        QLabel#heroSubtitle {
            color: #9fa8b2;
            font-size: 14px;
            line-height: 1.4;
        }
        QLabel#sectionTitle {
            color: #d7dce2;
            font-size: 13px;
            font-weight: 700;
            text-transform: uppercase;
            letter-spacing: 1.4px;
        }
        QLabel#stateBadge {
            background: #3a3024;
            border: 1px solid #7f6747;
            border-radius: 10px;
            color: #f4e9d9;
            font-size: 12px;
            font-weight: 700;
            letter-spacing: 1px;
            padding: 4px 10px;
            min-width: 92px;
        }
        QListWidget {
            background: #111418;
            border: 1px solid #313941;
            border-radius: 14px;
            padding: 8px;
            outline: none;
        }
        QListWidget::item {
            background: transparent;
            border-radius: 12px;
            padding: 12px 14px;
            margin: 3px 0;
        }
        QListWidget::item:selected {
            background: #2c343d;
            color: #f6efe4;
        }
        QListWidget::item:hover:!selected {
            background: #1e252c;
        }
        QPushButton {
            background: #29313a;
            border: 1px solid #39444f;
            border-radius: 10px;
            color: #eff2f5;
            padding: 8px 14px;
            font-weight: 600;
        }
        QPushButton:hover {
            background: #313b45;
        }
        QPushButton:disabled {
            color: #6e7882;
            border-color: #2a3036;
            background: #1d2227;
        }
        QPushButton#primaryButton {
            background: #c69c6d;
            color: #121416;
            border: none;
        }
        QPushButton#primaryButton:hover {
            background: #d8ab79;
        }
        QLineEdit,
        QComboBox,
        QPlainTextEdit {
            background: #0f1215;
            border: 1px solid #343c45;
            border-radius: 10px;
            padding: 8px 10px;
            min-height: 18px;
        }
        QLineEdit:focus,
        QComboBox:focus,
        QPlainTextEdit:focus {
            border: 1px solid #c69c6d;
        }
        QGroupBox {
            border: 1px solid #2f363e;
            border-radius: 14px;
            margin-top: 12px;
            padding: 16px 14px 14px 14px;
            font-weight: 700;
            color: #dfe5eb;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 12px;
            padding: 0 6px;
        }
    )"));

    QFont font(QStringLiteral("Inter"));
    font.setPointSize(10);
    app.setFont(font);
}
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName(QStringLiteral("rolaandjayz"));
    QCoreApplication::setOrganizationDomain(QStringLiteral("local"));
    QCoreApplication::setApplicationName(QStringLiteral("MY PLAITHROUGH"));
    QCoreApplication::setApplicationVersion(QStringLiteral("0.1.0"));

    applyEditorialTheme(app);

    MainWindow window;
    window.show();

    return app.exec();
}

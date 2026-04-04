#include <QApplication>
#include <QPalette>
#include <QColor>
#include "ConnectDialog.h"
#include "MainWindow.h"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setStyle("Fusion");

    QPalette dark;
    dark.setColor(QPalette::Window,          QColor(0x0F, 0x0F, 0x0F));
    dark.setColor(QPalette::WindowText,      QColor(0xE0, 0xE0, 0xE0));
    dark.setColor(QPalette::Base,            QColor(0x0F, 0x0F, 0x0F));
    dark.setColor(QPalette::AlternateBase,   QColor(0x1A, 0x1A, 0x1A));
    dark.setColor(QPalette::Text,            QColor(0xE0, 0xE0, 0xE0));
    dark.setColor(QPalette::Button,          QColor(0x1A, 0x1A, 0x1A));
    dark.setColor(QPalette::ButtonText,      QColor(0xE0, 0xE0, 0xE0));
    dark.setColor(QPalette::BrightText,      Qt::white);
    dark.setColor(QPalette::Highlight,       QColor(0x2A, 0x2A, 0x2A));
    dark.setColor(QPalette::HighlightedText, QColor(0xE0, 0xE0, 0xE0));
    app.setPalette(dark);

    ConnectDialog dlg;
    if (dlg.exec() != QDialog::Accepted) return 0;

    MainWindow win(dlg.serverUrl(), dlg.displayName());
    win.showMaximized();
    return app.exec();
}

#include "fileEditor.h"
#include <QWidget>
#include <QPlainTextEdit>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFrame>
#include <QWidget>
#include <QFont>
#include <QFontDatabase>
#include <QClipboard>
#include <QApplication>
#include <QStatusBar>
#include <Qsci/qsciscintilla.h>

fileEditor::fileEditor(QWidget *parent) : QWidget{parent}{
    QFont font;
    font.setFamilies({"Consolas", "Menlo"});
    font.setStyleHint(QFont::Monospace);

    root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    // Top bar with filename
    QFrame *topBar = new QFrame();
    topBar->setFixedHeight(56);
    topBar->setStyleSheet(
        "QFrame { background: #111111; border-bottom: 1px solid #1E1E1E; }");

    topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(20, 0, 20, 0);
    topLayout->setSpacing(10);
    topLayout->setAlignment(Qt::AlignHCenter);

    fileLabel = new QLabel("no file", topBar);
    fileLabel->setStyleSheet(
        "color: #E0E0E0; font-size: 14px; font-weight: 500; letter-spacing: 0.04em;");

    // QScintilla Editor
    editor = new QsciScintilla(this);
    editor->setFont(font); // font
    editor->setMarginsFont(font);

    editor->setMarginType(0, QsciScintilla::NumberMargin); // number margin
    editor->setMarginLineNumbers(0, true);
    editor->setMarginsForegroundColor(QColor(0xEE, 0xEE, 0xEE));
    editor->setMarginsBackgroundColor(QColor(0x11, 0x11, 0x11));
    editor->setMarginWidth(0, "00000");
    editor->setMarginType(1, QsciScintilla::SymbolMargin); // spacer margin
    editor->setMarginWidth(1, 18);

    editor->setCaretLineVisible(true); // cursor settings
    editor->setCaretWidth(2);
    editor->setCaretForegroundColor(QColor(0x11, 0x11, 0x11));
    editor->setCaretLineBackgroundColor(QColor(0x22, 0x22, 0x22));

    editor->setTabWidth(4);
    editor->setIndentationsUseTabs(false);
    editor->setBackspaceUnindents(true);
    editor->setAutoIndent(true);
    editor->setBraceMatching(QsciScintilla::SloppyBraceMatch);


    root->addWidget(topBar);
    topLayout->addWidget(fileLabel);
    root->addWidget(editor);
}

void fileEditor::loadFile(QFile *file, QString *filename){
    int i, j, isExt = 0;
    ext.resize(7); // can an extension be any longer than this?
    j = 0;
    if(file->open(QIODeviceBase::ReadOnly)){
        this->editor->read(file);
        this->fileLabel->setText(*filename);
    }
    for(i = 0; i < filename->length(); ++i){
        if(filename->at(i) == QChar('.'))
            isExt = 1;
        if(isExt){
            ext[j] = QChar(filename->at(i));
            ++j;
        }
    }
    // set the lexer here later

}
void fileEditor::newFile(QString *filename){
    fileLabel->setText(*filename);
    editor->clear();
}
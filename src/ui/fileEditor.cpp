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
    root = new QVBoxLayout(this);
    // Top bar with filename
    topBar = new QWidget();

    topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(16, 0, 16, 0);
    topLayout->setSpacing(10);

    divider = new QFrame(topBar);
    divider->setFixedSize(1,20);

    fileLabel = new QLabel("No file :(", topBar);

    // QScintilla Editor
    editor = new QsciScintilla(this);
    editor->setMarginType(0, QsciScintilla::NumberMargin);
    editor->setMarginLineNumbers(0, true);
    QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    // find a better font this is just a placeholder its so bad
    editor->setFont(font);
    editor->setBraceMatching(QsciScintilla::SloppyBraceMatch);
    editor->setMarginBackgroundColor(0, "111111");
    editor->setMarginWidth(0, "000");
    //HARDCODED!! ADD LINE NUMBER COUNTER WHEN FILES ARE ADDED

    root->addWidget(topBar);
    topLayout->addWidget(fileLabel);
    root->addWidget(editor);
}

//void fileEditor::loadFile(QFile *file, QString *filename){
//    if(file.open(QIODeviceBase::ReadOnly)){
//        this->editor->read(file);
//        this->root->topBar->topLayout->fileLabel->setText(filename);
//    }
//}
#ifndef FILEEDITOR_H
#define FILEEDITOR_H

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
#include <QFileInfo>
#include <Qsci/qsciscintilla.h>

class QVBoxlayout;
class QHBoxlayout;
class QLabel;
class QFrame;
class QsciScintilla;

class fileEditor : public QWidget
{
    Q_OBJECT
public:
    explicit fileEditor(QWidget *parent = nullptr);
signals:

public slots:
    void loadFile(QString &path); // uploads a file to the editor and updates header
    void newFile(QString *filename);

private:
    QVBoxLayout *root;
    QWidget *topBar;
    QHBoxLayout *topLayout;
    QLabel *fileLabel;
    QFrame *divider;
    QsciScintilla *editor;
    QFont font;
    QString ext; //file extension
};

#endif // FILEVIEWER_H

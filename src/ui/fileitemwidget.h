#ifndef FILEITEMWIDGET_H
#define FILEITEMWIDGET_H

#include <QWidget>
#include <QFile>
#include <QFileInfo>
#include <QLabel>
#include <QMouseEvent>

class QFile;

class fileItemWidget : public QWidget
{
    Q_OBJECT
public:
    explicit fileItemWidget(QWidget *parent = nullptr, QString path = "");

signals:
    void fileSelected(QString &path);
protected:
    void mouseReleaseEvent(QMouseEvent *event) override;
private:
    QString *filePath;
    QLabel *label;
};

#endif // FILEITEMWIDGET_H

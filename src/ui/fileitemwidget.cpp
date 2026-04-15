#include "fileitemwidget.h"
#include <QFileInfo>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

fileItemWidget::fileItemWidget(QWidget *parent, const QString &path)
    : QWidget(parent), filePath(path)
{
    QFileInfo info(path);   // was QFile — wrong class, showed full path

    setFixedHeight(36);
    setStyleSheet(
        "QWidget {"
        "  border-bottom: 1px solid #1A1A1A;"
        "  background: transparent;"
        "}");

    auto *l = new QVBoxLayout(this);
    l->setContentsMargins(16, 0, 16, 0);
    l->setSpacing(2);

    auto *topRow = new QHBoxLayout();
    topRow->setContentsMargins(0, 0, 0, 0);

    label = new QLabel(info.fileName(), this);
    label->setStyleSheet(
        "color: #C8C8C8; font-size: 13px; font-weight: 600; background: transparent;");

    l->addLayout(topRow);
    topRow->addWidget(label);
    setCursor(Qt::PointingHandCursor);
}

void fileItemWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        emit fileSelected(filePath);
    }
    QWidget::mouseReleaseEvent(event);
}

#include "fileitemwidget.h"
#include <QFile>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

fileItemWidget::fileItemWidget(QWidget *parent, QString path) : QWidget{parent}{
    this->filePath = new QString(path);
    QFile file(path);
    QFile info(path);

    setFixedHeight(36);
    setStyleSheet( "QWidget {" " border-bottom: 1px solid #1A1A1A;" " background: transparent;" "}");
    auto *l = new QVBoxLayout(this);
    l->setContentsMargins(16, 0, 16, 0);
    l->setSpacing(2);

    auto *topRow = new QHBoxLayout();
    topRow->setContentsMargins(0, 0, 0, 0);

    label = new QLabel("error", this);
    label->setText(info.fileName());
    label->setStyleSheet(
        "color: #C8C8C8; font-size: 13px; font-weight: 600; background: transparent;");
    l->addLayout(topRow);
    topRow->addWidget(label);
    setCursor(Qt::PointingHandCursor);
    // add delete button later
    // add rename later
}


// on click
void fileItemWidget::mouseReleaseEvent(QMouseEvent *event){
    if (event->button() == Qt::LeftButton) {
        emit fileSelected(* this->filePath);
    }
    QWidget::mouseReleaseEvent(event);
}

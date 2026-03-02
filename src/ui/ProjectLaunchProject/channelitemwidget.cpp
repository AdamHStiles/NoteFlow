#include "ChannelItemWidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

ChannelItemWidget::ChannelItemWidget(const QString &name, const QString &preview,
                                     int unread, QWidget *parent)
    : QWidget(parent)
{
    setFixedHeight(52);
    auto *l = new QVBoxLayout(this);
    l->setContentsMargins(16, 0, 16, 0);
    l->setSpacing(2);

    auto *topRow = new QHBoxLayout();
    topRow->setContentsMargins(0, 0, 0, 0);

    auto *nameLabel = new QLabel(name, this);
    nameLabel->setStyleSheet(
        "color: #C8C8C8; font-size: 13px; font-weight: 600; background: transparent;");
    topRow->addWidget(nameLabel);
    topRow->addStretch();

    if (unread > 0) {
        auto *badge = new QLabel(QString::number(unread), this);
        badge->setFixedSize(18, 18);
        badge->setAlignment(Qt::AlignCenter);
        badge->setStyleSheet(
            "background: #3A3A3A; color: #E0E0E0; border-radius: 9px; "
            "font-size: 10px; font-weight: 700;");
        topRow->addWidget(badge);
    }

    l->addLayout(topRow);

    if (!preview.isEmpty()) {
        auto *previewLabel = new QLabel(preview, this);
        previewLabel->setStyleSheet(
            "color: #3E3E3E; font-size: 11px; background: transparent;");
        previewLabel->setMaximumWidth(180);
        l->addWidget(previewLabel);
    }
}

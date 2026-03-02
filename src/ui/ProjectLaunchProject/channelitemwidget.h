#pragma once
#include <QWidget>

class ChannelItemWidget : public QWidget {
public:
    ChannelItemWidget(const QString &name, const QString &preview,int unread, QWidget *parent = nullptr);
};

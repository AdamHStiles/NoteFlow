#pragma once
#include <QWidget>
#include <QObject>

class ChannelItemWidget : public QWidget {
    Q_OBJECT

public:
    ChannelItemWidget(const QString &name, const QString &preview,int unread, QWidget *parent = nullptr);

signals:
    void addFile();
};

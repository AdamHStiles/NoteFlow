#pragma once
#include <QMainWindow>
#include <QMap>
#include <QVector>
#include <QStringList>
#include "Message.h"
#include <QProcess>
#include <QCoreApplication>

class WsClient;
class ChatView;
class QListWidget;
class QFrame;
class QLineEdit;
class QPushButton;
class QStackedWidget;
class QLabel;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(const QString &serverUrl, const QString &displayName,
               QWidget *parent = nullptr);

private:
    void buildUI();
    void addChannelToList(const QString &name);
    void showChannelInput(bool visible);

private slots:
    void onChannelChanged(int row);
    void onSendMessage(const QString &text);
    void onMessageReceived(const QString &channel, const Message &msg);
    void onWsConnected();
    void onWsDisconnected();
    void onWsError(const QString &reason);
    void onAddChannelConfirmed();
    void onRemoveCurrentChannel();

private:
    WsClient    *m_ws;
    ChatView    *m_chatView       = nullptr;
    QListWidget *m_channelList    = nullptr;
    QFrame      *m_statusDot      = nullptr;
    QLineEdit   *m_channelInput   = nullptr;
    QWidget     *m_channelInputWidget = nullptr;
    QStringList  m_channels;
    QString      m_activeChannel;
    QString      m_myName;
    QString      m_serverUrl;
    QMap<QString, QVector<Message>> m_messages;
};

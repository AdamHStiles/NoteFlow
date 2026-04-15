#pragma once
#include <QMainWindow>
#include <QMap>
#include <QVector>
#include <QStringList>
#include "Message.h"
#include "fileEditor.h"
#include "fileitemwidget.h"
#include <QProcess>
#include <QCoreApplication>
#include <QTreeWidget>

class WsClient;
class ChatView;
class QListWidget;
class QFrame;
class QLineEdit;
class QPushButton;
class QStackedWidget;
class QLabel;
class fileEditor;
class fileItemWidget;
class QTreeWidget;

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
    void onTreeItemClicked(QTreeWidgetItem *item, int col);
    void onSendMessage(const QString &text);
    void onMessageReceived(const QString &channel, const Message &msg);
    void onWsConnected();
    void onWsDisconnected();
    void onWsError(const QString &reason);
    void onAddChannelConfirmed();
    void onRemoveCurrentChannel();
    void onLocalFileChanged(const QString &filename, int position, int length,
                            const QString &text, bool isAddition);
    void onRemoteFileEdit(const QString &channel, const QString &filename,
                          int position, int length, const QString &text, bool isAddition);
    void onFileUploadReceived(const QString &channel, const QString &filename,
                              const QByteArray &content);

    // Persistence slots
    void onChannelListReceived(const QStringList &channels);
    void onChannelCreated(const QString &channel);
    void onMessageHistoryReceived(const QString &channel,
                                  const QVector<Message> &messages);

private:
    WsClient    *m_ws;
    ChatView    *m_chatView       = nullptr;
    QTreeWidget *m_channelTree    = nullptr;
    QFrame      *m_statusDot      = nullptr;
    QLineEdit   *m_channelInput   = nullptr;
    QWidget     *m_channelInputWidget = nullptr;
    QStringList  m_channels;
    QString      m_activeChannel;
    QString      m_myName;
    QString      m_serverUrl;
    QString      m_currentFile;
    fileEditor  *m_editor         = nullptr;
    QMap<QString, QVector<Message>> m_messages;
};

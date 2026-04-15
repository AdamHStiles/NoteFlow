#pragma once
#include <QObject>
#include <QUrl>
#include <QVector>
#include <QStringList>
#include <QtWebSockets/QWebSocket>
#include "Message.h"

class WsClient : public QObject {
    Q_OBJECT
public:
    explicit WsClient(const QString &displayName, QObject *parent = nullptr);

    void    connectToServer(const QUrl &url);
    void    disconnectFromServer();
    QString displayName() const;

    void sendMessage(const QString &channel, const QString &text);
    void joinChannel(const QString &channel);
    void sendChannelCreate(const QString &channel);

    void sendFileEdit(const QString &channel, const QString &filename,
                      int position, int length, const QString &text, bool isAddition);
    void sendFileUpload(const QString &channel, const QString &filename,
                        const QByteArray &content);

signals:
    void connected();
    void disconnected();
    void messageReceived(const QString &channel, const Message &msg);
    void presenceUpdate(const QString &channel, int count);
    void errorOccurred(const QString &reason);

    // Persistence signals
    void channelListReceived(const QStringList &channels);
    void channelCreated(const QString &channel);
    void messageHistoryReceived(const QString &channel, const QVector<Message> &messages);

    // File signals
    void fileEditReceived(const QString &channel, const QString &filename,
                          int position, int length, const QString &text, bool isAddition);
    void fileUploadReceived(const QString &channel, const QString &filename,
                            const QByteArray &content);

private slots:
    void onConnected();
    void onDisconnected();
    void onTextMessage(const QString &raw);
    void onError(QAbstractSocket::SocketError);

private:
    QWebSocket *m_ws;
    QString     m_name;
    QUrl        m_serverUrl;
};

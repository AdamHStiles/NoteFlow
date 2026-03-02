#pragma once
#include <QObject>
#include <QUrl>
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

signals:
    void connected();
    void disconnected();
    void messageReceived(const QString &channel, const Message &msg);
    void presenceUpdate(const QString &channel, int count);
    void errorOccurred(const QString &reason);

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

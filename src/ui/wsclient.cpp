#include "WsClient.h"

#include <QJsonObject>
#include <QJsonDocument>
#include <QDateTime>

WsClient::WsClient(const QString &displayName, QObject *parent)
    : QObject(parent), m_name(displayName)
{
    m_ws = new QWebSocket(QString(), QWebSocketProtocol::VersionLatest, this);
    connect(m_ws, &QWebSocket::connected,    this, &WsClient::onConnected);
    connect(m_ws, &QWebSocket::disconnected, this, &WsClient::onDisconnected);
    connect(m_ws, &QWebSocket::textMessageReceived, this, &WsClient::onTextMessage);
    connect(m_ws,
            QOverload<QAbstractSocket::SocketError>::of (&QWebSocket::error),
            this, &WsClient::onError);
}

void WsClient::connectToServer(const QUrl &url)    { m_serverUrl = url; m_ws->open(url); }
void WsClient::disconnectFromServer()               { m_ws->close(); }
QString WsClient::displayName() const               { return m_name; }

void WsClient::sendMessage(const QString &channel, const QString &text)
{
    QJsonObject obj;
    obj["type"]    = "message";
    obj["channel"] = channel;
    obj["sender"]  = m_name;
    obj["text"]    = text;
    obj["ts"]      = QDateTime::currentDateTime().toString("hh:mm");
    m_ws->sendTextMessage(QJsonDocument(obj).toJson(QJsonDocument::Compact));
}

void WsClient::joinChannel(const QString &channel)
{
    QJsonObject obj;
    obj["type"]    = "join";
    obj["channel"] = channel;
    obj["sender"]  = m_name;
    m_ws->sendTextMessage(QJsonDocument(obj).toJson(QJsonDocument::Compact));
}

void WsClient::onConnected()    { emit connected(); }
void WsClient::onDisconnected() { emit disconnected(); }

void WsClient::onTextMessage(const QString &raw)
{
    QJsonParseError err;
    auto doc = QJsonDocument::fromJson(raw.toUtf8(), &err);
    if (err.error != QJsonParseError::NoError || !doc.isObject()) return;

    QJsonObject obj  = doc.object();
    QString     type = obj["type"].toString();

    if (type == "message") {
        Message msg;
        msg.sender    = obj["sender"].toString();
        msg.text      = obj["text"].toString();
        msg.timestamp = obj.contains("ts")
                            ? obj["ts"].toString()
                            : QDateTime::currentDateTime().toString("hh:mm");
        msg.isMe      = (msg.sender == m_name);
        emit messageReceived(obj["channel"].toString(), msg);
    }
    else if (type == "presence") {
        emit presenceUpdate(obj["channel"].toString(), obj["count"].toInt());
    }
}

void WsClient::onError(QAbstractSocket::SocketError)
{
    emit errorOccurred(m_ws->errorString());
}

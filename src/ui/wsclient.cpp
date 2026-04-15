#include "WsClient.h"

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDateTime>

WsClient::WsClient(const QString &displayName, QObject *parent)
    : QObject(parent), m_name(displayName)
{
    m_ws = new QWebSocket(QString(), QWebSocketProtocol::VersionLatest, this);
    connect(m_ws, &QWebSocket::connected,    this, &WsClient::onConnected);
    connect(m_ws, &QWebSocket::disconnected, this, &WsClient::onDisconnected);
    connect(m_ws, &QWebSocket::textMessageReceived, this, &WsClient::onTextMessage);
    connect(m_ws,
            QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error),
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

void WsClient::sendChannelCreate(const QString &channel)
{
    QJsonObject obj;
    obj["type"]    = "channel_create";
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

    // ── Channel list (sent by server on connect) ─────────────────────────
    if (type == "channel_list") {
        QJsonArray arr = obj["channels"].toArray();
        QStringList names;
        for (const auto &v : arr)
            names.append(v.toString());
        emit channelListReceived(names);
    }

    // ── Channel created by another user ──────────────────────────────────
    else if (type == "channel_create") {
        emit channelCreated(obj["channel"].toString());
    }

    // ── Message history (sent by server on join) ─────────────────────────
    else if (type == "message_history") {
        QString channel = obj["channel"].toString();
        QJsonArray arr  = obj["messages"].toArray();
        QVector<Message> messages;

        for (const auto &v : arr) {
            QJsonObject m = v.toObject();
            Message msg;
            msg.sender    = m["sender"].toString();
            msg.text      = m["text"].toString();
            msg.timestamp = m["ts"].toString();
            msg.isMe      = (msg.sender == m_name);
            messages.append(msg);
        }

        emit messageHistoryReceived(channel, messages);
    }

    // ── Live chat message ────────────────────────────────────────────────
    else if (type == "message") {
        if (obj["sender"].toString() == m_name) return;

        Message msg;
        msg.sender    = obj["sender"].toString();
        msg.text      = obj["text"].toString();
        msg.timestamp = obj["ts"].toString();
        msg.isMe      = false;

        emit messageReceived(obj["channel"].toString(), msg);
    }

    // ── File edit ────────────────────────────────────────────────────────
    else if (type == "file_edit") {
        if (obj["sender"].toString() == m_name) return;

        emit fileEditReceived(
            obj["channel"].toString(),
            obj["filename"].toString(),
            obj["position"].toInt(),
            obj["length"].toInt(),
            obj["text"].toString(),
            obj["isAddition"].toBool()
            );
    }

    // ── File upload ──────────────────────────────────────────────────────
    else if (type == "file_upload") {
        // No sender check here — the server already excludes us from live
        // broadcasts, and on join we need to receive ALL files including
        // ones we originally uploaded.

        QByteArray content = QByteArray::fromBase64(
            obj["content"].toString().toLatin1());

        emit fileUploadReceived(
            obj["channel"].toString(),
            obj["filename"].toString(),
            content
            );
    }
}

void WsClient::onError(QAbstractSocket::SocketError)
{
    emit errorOccurred(m_ws->errorString());
}

void WsClient::sendFileEdit(const QString &channel, const QString &filename,
                            int position, int length, const QString &text,
                            bool isAddition)
{
    QJsonObject obj;
    obj["type"]       = "file_edit";
    obj["channel"]    = channel;
    obj["sender"]     = m_name;
    obj["filename"]   = filename;
    obj["position"]   = position;
    obj["length"]     = length;
    obj["text"]       = text;
    obj["isAddition"] = isAddition;

    m_ws->sendTextMessage(QJsonDocument(obj).toJson(QJsonDocument::Compact));
}

void WsClient::sendFileUpload(const QString &channel, const QString &filename,
                              const QByteArray &content)
{
    QJsonObject obj;
    obj["type"]     = "file_upload";
    obj["channel"]  = channel;
    obj["sender"]   = m_name;
    obj["filename"] = filename;
    obj["content"]  = QString::fromLatin1(content.toBase64());

    m_ws->sendTextMessage(QJsonDocument(obj).toJson(QJsonDocument::Compact));
}

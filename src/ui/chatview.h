#pragma once
#include <QWidget>
#include <QVector>
#include "Message.h"
#include <QVector>

class QVBoxLayout;
class QFrame;
class QLabel;
class QScrollArea;
class QLineEdit;
class QPushButton;

class ChatView : public QWidget {
    Q_OBJECT
public:
    explicit ChatView(QWidget *parent = nullptr);

    void loadChannel(const QString &name, QList<Message> &messages);
    void appendMessage(const Message &msg);
    void setInputEnabled(bool enabled);

signals:
    void messageSent(const QString &text);

private slots:
    void onSendClicked();
    void scrollToBottom();

private:
    QVBoxLayout  *m_layout;
    QFrame       *m_header;
    QLabel       *m_channelLabel;
    QScrollArea  *m_scroll;
    QWidget      *m_msgContainer;
    QVBoxLayout  *m_msgLayout;
    QLineEdit    *m_input;
    QPushButton  *m_sendBtn;
    QList<Message> *m_currentMessages = nullptr;
};

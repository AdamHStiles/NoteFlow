#include "ChatView.h"
#include "MessageBubble.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QScrollArea>
#include <QScrollBar>
#include <QLineEdit>
#include <QPushButton>
#include <QTimer>

ChatView::ChatView(QWidget *parent) : QWidget(parent)
{
    setStyleSheet("background: #0F0F0F;");

    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);

    // Header
    m_header = new QFrame(this);
    m_header->setFixedHeight(56);
    m_header->setStyleSheet(
        "QFrame { background: #111111; border-bottom: 1px solid #1E1E1E; }");
    auto *hl = new QHBoxLayout(m_header);
    hl->setContentsMargins(20, 0, 20, 0);

    m_channelLabel = new QLabel("", m_header);
    m_channelLabel->setStyleSheet(
        "color: #E0E0E0; font-size: 14px; font-weight: 700; letter-spacing: 0.04em;");
    hl->addWidget(m_channelLabel);
    m_layout->addWidget(m_header);

    // Scroll area
    m_scroll = new QScrollArea(this);
    m_scroll->setWidgetResizable(true);
    m_scroll->setFrameShape(QFrame::NoFrame);
    m_scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scroll->setStyleSheet(
        "QScrollArea { background: #0F0F0F; border: none; }"
        "QScrollBar:vertical { background: #111111; width: 4px; border-radius: 2px; }"
        "QScrollBar::handle:vertical { background: #2A2A2A; border-radius: 2px; min-height: 20px; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; }");

    m_msgContainer = new QWidget();
    m_msgContainer->setStyleSheet("background: #0F0F0F;");
    m_msgLayout = new QVBoxLayout(m_msgContainer);
    m_msgLayout->setContentsMargins(0, 16, 0, 16);
    m_msgLayout->setSpacing(6);
    m_msgLayout->addStretch();

    m_scroll->setWidget(m_msgContainer);
    m_layout->addWidget(m_scroll);

    auto *div = new QFrame(this);
    div->setFixedHeight(1);
    div->setStyleSheet("background: #1A1A1A;");
    m_layout->addWidget(div);

    // Input area
    auto *inputArea = new QWidget(this);
    inputArea->setFixedHeight(64);
    inputArea->setStyleSheet("background: #111111;");
    auto *il = new QHBoxLayout(inputArea);
    il->setContentsMargins(16, 12, 16, 12);
    il->setSpacing(10);

    m_input = new QLineEdit(inputArea);
    m_input->setPlaceholderText("Message...");
    m_input->setFixedHeight(36);
    m_input->setEnabled(false);
    m_input->setStyleSheet(
        "QLineEdit {"
        "  background: #1A1A1A; border: 1px solid #252525; border-radius: 6px;"
        "  color: #E0E0E0; font-size: 13px; padding: 0 12px;"
        "}"
        "QLineEdit:focus { border-color: #383838; background: #1E1E1E; }"
        "QLineEdit:disabled { color: #3A3A3A; background: #141414; border-color: #1E1E1E; }"
        "QLineEdit::placeholder { color: #3A3A3A; }");

    m_sendBtn = new QPushButton("Send", inputArea);
    m_sendBtn->setFixedSize(64, 36);
    m_sendBtn->setEnabled(false);
    m_sendBtn->setCursor(Qt::PointingHandCursor);
    m_sendBtn->setStyleSheet(
        "QPushButton {"
        "  background: #2A2A2A; border: 1px solid #353535; border-radius: 6px;"
        "  color: #C0C0C0; font-size: 12px; font-weight: 600; letter-spacing: 0.05em;"
        "}"
        "QPushButton:hover { background: #333333; border-color: #404040; color: #E0E0E0; }"
        "QPushButton:pressed { background: #222222; }"
        "QPushButton:disabled { color: #2E2E2E; border-color: #1E1E1E; background: #181818; }");

    il->addWidget(m_input);
    il->addWidget(m_sendBtn);
    m_layout->addWidget(inputArea);

    connect(m_sendBtn, &QPushButton::clicked,     this, &ChatView::onSendClicked);
    connect(m_input,   &QLineEdit::returnPressed, this, &ChatView::onSendClicked);
}

void ChatView::loadChannel(const QString &name, QList<Message> &messages)
{
    m_channelLabel->setText(name);
    m_currentMessages = &messages;

    while (m_msgLayout->count() > 1) {
        QLayoutItem *item = m_msgLayout->takeAt(1);
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }
    for (auto &msg : messages)
        m_msgLayout->addWidget(new MessageBubble(msg, m_msgContainer));

    QTimer::singleShot(50, this, &ChatView::scrollToBottom);
}

void ChatView::appendMessage(const Message &msg)
{
    if (m_currentMessages) m_currentMessages->append(msg);
    m_msgLayout->addWidget(new MessageBubble(msg, m_msgContainer));
    QTimer::singleShot(50, this, &ChatView::scrollToBottom);
}

void ChatView::setInputEnabled(bool enabled)
{
    m_input->setEnabled(enabled);
    m_sendBtn->setEnabled(enabled);
}

void ChatView::onSendClicked()
{
    QString text = m_input->text().trimmed();
    if (text.isEmpty()) return;
    m_input->clear();
    emit messageSent(text);
}

void ChatView::scrollToBottom()
{
    m_scroll->verticalScrollBar()->setValue(
        m_scroll->verticalScrollBar()->maximum());
}

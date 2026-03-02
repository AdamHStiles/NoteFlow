#include "MessageBubble.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QTimer>

MessageBubble::MessageBubble(const Message &msg, QWidget *parent)
    : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    auto *outer = new QHBoxLayout(this);
    outer->setContentsMargins(16, 4, 16, 4);

    auto *col = new QVBoxLayout();
    col->setSpacing(2);

    if (!msg.isMe) {
        auto *senderLabel = new QLabel(msg.sender, this);
        senderLabel->setStyleSheet(
            "color: #606060; font-size: 11px; font-weight: 600; letter-spacing: 0.08em;");
        col->addWidget(senderLabel);
    }

    auto *bubble = new QFrame(this);
    bubble->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum);
    auto *bl = new QVBoxLayout(bubble);
    bl->setContentsMargins(12, 8, 12, 8);
    bl->setSpacing(4);

    auto *textLabel = new QLabel(msg.text, bubble);
    textLabel->setWordWrap(true);
    textLabel->setMaximumWidth(480);
    textLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    if (msg.isMe) {
        bubble->setStyleSheet(
            "QFrame { background: #2A2A2A; border-radius: 12px; border-top-right-radius: 3px; }");
        textLabel->setStyleSheet("color: #E8E8E8; font-size: 13px;");
    } else {
        bubble->setStyleSheet(
            "QFrame { background: #1E1E1E; border-radius: 12px; border-top-left-radius: 3px; "
            "border: 1px solid #2A2A2A; }");
        textLabel->setStyleSheet("color: #C8C8C8; font-size: 13px;");
    }

    auto *timeLabel = new QLabel(msg.timestamp, bubble);
    timeLabel->setStyleSheet("color: #444444; font-size: 10px;");
    timeLabel->setAlignment(Qt::AlignRight);

    bl->addWidget(textLabel);
    bl->addWidget(timeLabel);
    col->addWidget(bubble, 0, msg.isMe ? Qt::AlignRight : Qt::AlignLeft);

    if (msg.isMe) { outer->addStretch(); outer->addLayout(col); }
    else           { outer->addLayout(col); outer->addStretch(); }

    auto *effect = new QGraphicsOpacityEffect(this);
    effect->setOpacity(0.0);
    setGraphicsEffect(effect);
    auto *anim = new QPropertyAnimation(effect, "opacity", this);
    anim->setDuration(180);
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    QTimer::singleShot(10, anim, [anim]() { anim->start(); });
}

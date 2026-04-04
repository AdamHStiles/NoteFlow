#pragma once
#include <QWidget>
#include "Message.h"

class MessageBubble : public QWidget {
public:
    explicit MessageBubble(const Message &msg, QWidget *parent = nullptr);
};

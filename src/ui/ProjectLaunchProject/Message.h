#pragma once
#include <QString>

struct Message {
    QString sender;
    QString text;
    QString timestamp;
    bool    isMe;
};

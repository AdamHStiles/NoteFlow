#include "ConnectDialog.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

ConnectDialog::ConnectDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("Connect");
    setFixedSize(380, 220);
    setStyleSheet(
        "QDialog { background: #111111; }"
        "QLabel { color: #888888; font-size: 11px; letter-spacing: 0.08em; }"
        "QLineEdit {"
        "  background: #1A1A1A; border: 1px solid #252525; border-radius: 6px;"
        "  color: #E0E0E0; font-size: 13px; padding: 0 12px; height: 36px;"
        "}"
        "QLineEdit:focus { border-color: #404040; }"
        "QLineEdit::placeholder { color: #333333; }");

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(28, 28, 28, 28);
    layout->setSpacing(0);

    auto *title = new QLabel("connect to server", this);
    title->setStyleSheet(
        "color: #E0E0E0; font-size: 15px; font-weight: 700; letter-spacing: 0.04em;");
    layout->addWidget(title);
    layout->addSpacing(24);

    layout->addWidget(new QLabel("SERVER URL", this));
    layout->addSpacing(6);

    m_urlInput = new QLineEdit(this);
    m_urlInput->setPlaceholderText("ws://your-server:port");
    m_urlInput->setFixedHeight(36);
    layout->addWidget(m_urlInput);
    layout->addSpacing(14);

    layout->addWidget(new QLabel("DISPLAY NAME", this));
    layout->addSpacing(6);

    m_nameInput = new QLineEdit(this);
    m_nameInput->setPlaceholderText("username");
    m_nameInput->setFixedHeight(36);
    layout->addWidget(m_nameInput);
    layout->addSpacing(20);

    auto *btn = new QPushButton("Connect", this);
    btn->setFixedHeight(38);
    btn->setCursor(Qt::PointingHandCursor);
    btn->setStyleSheet(
        "QPushButton {"
        "  background: #2A2A2A; border: 1px solid #383838; border-radius: 6px;"
        "  color: #E0E0E0; font-size: 13px; font-weight: 600; letter-spacing: 0.05em;"
        "}"
        "QPushButton:hover { background: #333333; }"
        "QPushButton:pressed { background: #222222; }");
    layout->addWidget(btn);

    connect(btn,         &QPushButton::clicked,     this, &ConnectDialog::onConnect);
    connect(m_nameInput, &QLineEdit::returnPressed, this, &ConnectDialog::onConnect);
}

QString ConnectDialog::serverUrl()   const { return m_urlInput->text().trimmed(); }
QString ConnectDialog::displayName() const { return m_nameInput->text().trimmed(); }

void ConnectDialog::onConnect()
{
    if (m_urlInput->text().trimmed().isEmpty() ||
        m_nameInput->text().trimmed().isEmpty()) return;
    accept();
}

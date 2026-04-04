#include "MainWindow.h"
#include "WsClient.h"
#include "ChatView.h"
#include "ChannelItemWidget.h"
#include "fileEditor.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListWidget>
#include <QListWidgetItem>
#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QStatusBar>
#include <QTimer>
#include <QDateTime>
#include <QSplitter>

MainWindow::MainWindow(const QString &serverUrl, const QString &displayName, QWidget *parent)
    : QMainWindow(parent), m_myName(displayName), m_serverUrl(serverUrl)
{
    setWindowTitle("NoteFlow");
    setMinimumSize(860, 580);
    resize(1100, 680);

    m_ws = new WsClient(displayName, this);
    connect(m_ws, &WsClient::connected,       this, &MainWindow::onWsConnected);
    connect(m_ws, &WsClient::disconnected,    this, &MainWindow::onWsDisconnected);
    connect(m_ws, &WsClient::messageReceived, this, &MainWindow::onMessageReceived);
    connect(m_ws, &WsClient::errorOccurred,   this, &MainWindow::onWsError);

    buildUI();

    statusBar()->setStyleSheet(
        "QStatusBar { background: #0D0D0D; color: #444444; font-size: 11px; "
        "border-top: 1px solid #1A1A1A; }");
    statusBar()->showMessage("Connecting to " + serverUrl + " ...");

    m_ws->connectToServer(QUrl(serverUrl));
}

void MainWindow::buildUI()
{
    auto *central = new QWidget(this);
    setCentralWidget(central);
    central->setStyleSheet("background: #0A0A0A;");

    auto *root = new QHBoxLayout(central);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // ── Sidebar ──────────────────────────────
    auto *sidebar = new QWidget(central);
    sidebar->setFixedWidth(220);
    sidebar->setStyleSheet(
        "QWidget#sidebar { background: #0D0D0D; border-right: 1px solid #1A1A1A; }");
    sidebar->setObjectName("sidebar");

    auto *sl = new QVBoxLayout(sidebar);
    sl->setContentsMargins(0, 0, 0, 0);
    sl->setSpacing(0);

    // ── Section header with + button ──
    auto *sectionHeader = new QWidget(sidebar);
    sectionHeader->setFixedHeight(56);
    sectionHeader->setObjectName("sectionHeader");
    sectionHeader->setStyleSheet("#sectionHeader { background: #111111; border-bottom: 1px solid #1E1E1E; border-right: 1px solid #1E1E1E}");

    auto *shl = new QHBoxLayout(sectionHeader);
    shl->setContentsMargins(16, 0, 16, 0);

    auto *sectionLabel = new QLabel("CHANNELS", sectionHeader);
    sectionLabel->setStyleSheet(
        "color: #333333; font-size: 10px; font-weight: 600; "
        "letter-spacing: 0.12em; background: transparent;");

    auto *addBtn = new QPushButton("+", sectionHeader);
    addBtn->setFixedSize(20, 20);
    addBtn->setCursor(Qt::PointingHandCursor);
    addBtn->setStyleSheet(
        "QPushButton {"
        "  background: transparent; border: 1px solid #2A2A2A;"
        "  border-radius: 3px; color: #444444; font-size: 14px; font-weight: 400;"
        "}"
        "QPushButton:hover { border-color: #404040; color: #888888; }"
        "QPushButton:pressed { background: #1A1A1A; }");

    shl->addWidget(sectionLabel);
    shl->addStretch();
    shl->addWidget(addBtn);

    sl->addWidget(sectionHeader);


    // ── Inline channel input (hidden by default) ──
    m_channelInputWidget = new QWidget(sidebar);
    m_channelInputWidget->setFixedHeight(52);
    m_channelInputWidget->setStyleSheet("background: #0D0D0D;");
    m_channelInputWidget->setVisible(false);
    auto *cil = new QHBoxLayout(m_channelInputWidget);
    cil->setContentsMargins(12, 8, 12, 8);
    cil->setSpacing(6);

    m_channelInput = new QLineEdit(m_channelInputWidget);
    m_channelInput->setPlaceholderText("channel-name");
    m_channelInput->setFixedHeight(30);
    m_channelInput->setMaxLength(32);
    m_channelInput->setStyleSheet(
        "QLineEdit {"
        "  background: #1A1A1A; border: 1px solid #383838; border-radius: 4px;"
        "  color: #E0E0E0; font-size: 12px; padding: 0 8px;"
        "}"
        "QLineEdit:focus { border-color: #505050; }"
        "QLineEdit::placeholder { color: #333333; }");

    auto *confirmBtn = new QPushButton("Add", m_channelInputWidget);
    confirmBtn->setFixedSize(36, 30);
    confirmBtn->setCursor(Qt::PointingHandCursor);
    confirmBtn->setStyleSheet(
        "QPushButton {"
        "  background: #2A2A2A; border: 1px solid #404040; border-radius: 4px;"
        "  color: #C0C0C0; font-size: 11px; font-weight: 600;"
        "}"
        "QPushButton:hover { background: #333333; color: #E0E0E0; }"
        "QPushButton:pressed { background: #222222; }");

    cil->addWidget(m_channelInput);
    cil->addWidget(confirmBtn);
    sl->addWidget(m_channelInputWidget);

    // ── Channel list ──
    m_channelList = new QListWidget(sidebar);
    m_channelList->setFrameShape(QFrame::NoFrame);
    m_channelList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_channelList->setStyleSheet(
        "QListWidget {"
        "  background: #0D0D0D; border: none; outline: none;"
        "}"
        "QListWidget::item {"
        "  border: none; padding: 0px; background: transparent;"
        "}"
        "QListWidget::item:selected {"
        "  background: #161616; border-left: 2px solid #505050;"
        "}"
        "QListWidget::item:hover:!selected {"
        "  background: #111111;"
        "}"
        "QScrollBar:vertical { width: 0px; }");
    m_channelList->setSpacing(0);

    sl->addWidget(m_channelList);
    sl->addStretch();

    // ── User footer ──
    auto *footer = new QFrame(sidebar);
    footer->setFixedHeight(65);
    footer->setObjectName("footer");
    footer->setStyleSheet(
        "#footer { background: #111111; border-top: 1px solid #1A1A1A; border-right: 1px solid #1A1A1A;}");
    auto *fl = new QHBoxLayout(footer);
    fl->setContentsMargins(16, 0, 16, 0);
    fl->setSpacing(0);

    m_statusDot = new QFrame(footer);
    m_statusDot->setFixedSize(8, 8);
    m_statusDot->setStyleSheet("background: #3A3A3A; border-radius: 4px;");

    auto *userNameLabel = new QLabel(m_myName, footer);
    userNameLabel->setStyleSheet(
        "color: #888888; font-size: 12px; font-weight: 600; "
        "letter-spacing: 0.05em; background: transparent;");

    fl->addWidget(m_statusDot);
    fl->addSpacing(8);
    fl->addWidget(userNameLabel);
    fl->addStretch();
    sl->addWidget(footer);

    fileEditor *editor = new fileEditor(this);

    // ── Chat View ────────────────────────────
    m_chatView = new ChatView(central);
    connect(m_chatView, &ChatView::messageSent, this, &MainWindow::onSendMessage);

    root->addWidget(sidebar);
    root->addWidget(editor);
    root->addWidget(m_chatView);

    // ── Connections ──────────────────────────
    connect(addBtn, &QPushButton::clicked, this, [this]() {
        showChannelInput(!m_channelInputWidget->isVisible());
    });
    connect(confirmBtn,    &QPushButton::clicked,     this, &MainWindow::onAddChannelConfirmed);
    connect(m_channelInput,&QLineEdit::returnPressed, this, &MainWindow::onAddChannelConfirmed);
    connect(m_channelList, &QListWidget::currentRowChanged,
            this, &MainWindow::onChannelChanged);
}

void MainWindow::showChannelInput(bool visible)
{
    m_channelInputWidget->setVisible(visible);
    if (visible) {
        m_channelInput->clear();
        m_channelInput->setFocus();
    }
}

void MainWindow::addChannelToList(const QString &name)
{
    // contains data of the new channel
    auto *item = new QListWidgetItem(m_channelList);
    item->setSizeHint(QSize(220, 52));
    item->setData(Qt::UserRole, name);
    item->setBackground(Qt::transparent);

    // this the actual widget
    auto *itemWidget = new ChannelItemWidget(name, "", 0, m_channelList);
    m_channelList->setItemWidget(item, itemWidget);

    connect(itemWidget, &ChannelItemWidget::actionClicked, this, [this, name]() {
        QString viewer = QCoreApplication::applicationDirPath() + "/CodeViewer.exe";
        QProcess::startDetached(viewer, {});

    });
}

void MainWindow::onAddChannelConfirmed()
{
    QString name = m_channelInput->text().trimmed().toLower();
    name.replace(" ", "-");
    if (name.isEmpty() || m_channels.contains(name)) {
        showChannelInput(false);
        return;
    }

    m_channels.append(name);
    addChannelToList(name);
    showChannelInput(false);

    // Select the new channel
    m_channelList->setCurrentRow(m_channelList->count() - 1);
    m_ws->joinChannel(name);
}

void MainWindow::onRemoveCurrentChannel()
{
    int row = m_channelList->currentRow();
    if (row < 0) return;
    QString name = m_channels[row];
    m_channels.removeAt(row);
    delete m_channelList->takeItem(row);
    m_messages.remove(name);

    if (!m_channels.isEmpty())
        m_channelList->setCurrentRow(qMin(row, m_channels.size() - 1));
    else
        m_activeChannel.clear();
}

void MainWindow::onChannelChanged(int row)
{
    if (row < 0 || row >= m_channels.size()) return;
    m_activeChannel = m_channels[row];
    m_chatView->loadChannel(m_activeChannel, m_messages[m_activeChannel]);
    m_ws->joinChannel(m_activeChannel);
}

void MainWindow::onSendMessage(const QString &text)
{
    if (m_activeChannel.isEmpty()) return;

    Message msg;
    msg.sender    = m_myName;
    msg.text      = text;
    msg.timestamp = QDateTime::currentDateTime().toString("hh:mm");
    msg.isMe      = true;

    m_messages[m_activeChannel].append(msg);
    m_chatView->appendMessage(msg);
    m_ws->sendMessage(m_activeChannel, text);
}

void MainWindow::onMessageReceived(const QString &channel, const Message &msg)
{
    if (msg.isMe) return;
    m_messages[channel].append(msg);
    if (channel == m_activeChannel)
        m_chatView->appendMessage(msg);
}

void MainWindow::onWsConnected()
{
    m_statusDot->setStyleSheet("background: #4CAF50; border-radius: 4px;");
    m_chatView->setInputEnabled(true);
    statusBar()->showMessage("Connected  —  " + m_myName);
    if (!m_activeChannel.isEmpty())
        m_ws->joinChannel(m_activeChannel);
}

void MainWindow::onWsDisconnected()
{
    m_statusDot->setStyleSheet("background: #3A3A3A; border-radius: 4px;");
    m_chatView->setInputEnabled(false);
    statusBar()->showMessage("Disconnected  —  retrying in 3s...");
    QTimer::singleShot(3000, this, [this]() {
        statusBar()->showMessage("Reconnecting...");
        m_ws->connectToServer(QUrl(m_serverUrl));
    });
}

void MainWindow::onWsError(const QString &reason)
{
    statusBar()->showMessage("Error: " + reason);
}

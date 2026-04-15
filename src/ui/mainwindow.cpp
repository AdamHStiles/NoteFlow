#include "mainwindow.h"
#include "WsClient.h"
#include "ChatView.h"
#include "ChannelItemWidget.h"
#include "fileEditor.h"
#include "fileitemwidget.h"

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
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QDebug>
#include <Qsci/qsciscintilla.h>
#include "QFileDialog"


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

    // Persistence signals
    connect(m_ws, &WsClient::channelListReceived,    this, &MainWindow::onChannelListReceived);
    connect(m_ws, &WsClient::channelCreated,         this, &MainWindow::onChannelCreated);
    connect(m_ws, &WsClient::messageHistoryReceived, this, &MainWindow::onMessageHistoryReceived);

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

    // ── Channel Section header with + button ──
    auto *sectionHeader = new QWidget(sidebar);
    sectionHeader->setFixedHeight(56);
    sectionHeader->setObjectName("sectionHeader");
    sectionHeader->setStyleSheet(
        "#sectionHeader { background: #111111; border-bottom: 1px solid #1E1E1E; "
        "border-right: 1px solid #1E1E1E; }");

    auto *shl = new QHBoxLayout(sectionHeader);
    shl->setContentsMargins(16, 0, 16, 0);

    auto *sectionLabel = new QLabel("CHANNELS", sectionHeader);
    sectionLabel->setStyleSheet(
        "color: #E0E0E0; font-size: 10px; font-weight: 600; "
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

    // ── Channel Tree ──
    m_channelTree = new QTreeWidget(sidebar);
    m_channelTree->setFrameShape(QFrame::NoFrame);
    m_channelTree->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_channelTree->setIndentation(15);
    m_channelTree->setHeaderHidden(true);
    m_channelTree->setStyleSheet(
        "QTreeWidget {"
        "  background: #0D0D0D; border: none; outline: none;"
        "}"
        "QTreeWidget::item {"
        "  border: none; padding: 0px; background: transparent;"
        "}"
        "QTreeWidget::item:selected {"
        "  background: #161616; border-left: 2px solid #505050;"
        "}"
        "QTreeWidget::item:hover:!selected {"
        "  background: #111111;"
        "}"
        "QScrollBar:vertical { width: 0px; }");

    sl->addWidget(m_channelTree);
    sl->addStretch();

    // ── User footer ──
    auto *footer = new QFrame(sidebar);
    footer->setFixedHeight(65);
    footer->setObjectName("footer");
    footer->setStyleSheet(
        "#footer { background: #111111; border-top: 1px solid #1A1A1A; "
        "border-right: 1px solid #1A1A1A; }");
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

    // ── Editor and splitter ──
    QSplitter *splitter = new QSplitter(Qt::Horizontal);
    m_editor = new fileEditor(this);

    // ── Chat View ──
    m_chatView = new ChatView(central);
    connect(m_chatView, &ChatView::messageSent, this, &MainWindow::onSendMessage);

    root->addWidget(splitter);
    splitter->addWidget(sidebar);
    splitter->addWidget(m_editor);
    splitter->addWidget(m_chatView);

    // ── Connections ──
    connect(addBtn, &QPushButton::clicked, this, [this]() {
        showChannelInput(!m_channelInputWidget->isVisible());
    });
    connect(confirmBtn,     &QPushButton::clicked,     this, &MainWindow::onAddChannelConfirmed);
    connect(m_channelInput, &QLineEdit::returnPressed, this, &MainWindow::onAddChannelConfirmed);
    connect(m_channelTree,  &QTreeWidget::itemClicked,
            this, &MainWindow::onTreeItemClicked);

    // ── File sync connections ──
    connect(m_ws,     &WsClient::fileEditReceived,   this, &MainWindow::onRemoteFileEdit);
    connect(m_ws,     &WsClient::fileUploadReceived, this, &MainWindow::onFileUploadReceived);
    connect(m_editor, &fileEditor::localFileChanged, this, &MainWindow::onLocalFileChanged);
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
    auto *channelItem = new QTreeWidgetItem(m_channelTree);
    channelItem->setData(0, Qt::UserRole, "CHANNEL:" + name);

    auto *itemWidget = new ChannelItemWidget(name, "", 0, m_channelTree);
    m_channelTree->setItemWidget(channelItem, 0, itemWidget);

    connect(itemWidget, &ChannelItemWidget::addFile, this, [this, channelItem, name]() {
        QString filePath = QFileDialog::getOpenFileName(this, "Upload file to " + name);
        if (!filePath.isEmpty()) {
            auto *fileItem = new QTreeWidgetItem(channelItem);
            fileItem->setData(0, Qt::UserRole, "FILE:" + filePath);
            auto *fileWidget = new fileItemWidget(m_channelTree, filePath);
            m_channelTree->setItemWidget(fileItem, 0, fileWidget);
            channelItem->setExpanded(true);

            connect(fileWidget, &fileItemWidget::fileSelected,
                    this, [this](const QString &path) {
                        m_editor->loadFile(path);
                        m_currentFile = QFileInfo(path).fileName();
                    });

            // Broadcast the file to everyone in the channel
            QFile file(filePath);
            if (file.open(QIODevice::ReadOnly)) {
                QByteArray content = file.readAll();
                file.close();
                m_ws->sendFileUpload(name, QFileInfo(filePath).fileName(), content);
            }
        }
    });
}

// ── Persistence handlers ─────────────────────────────────────────────────────

void MainWindow::onChannelListReceived(const QStringList &channels)
{
    // Server sends this on connect — populate sidebar with persisted channels
    for (const QString &name : channels) {
        if (!m_channels.contains(name)) {
            m_channels.append(name);
            addChannelToList(name);
        }
    }
}

void MainWindow::onChannelCreated(const QString &channel)
{
    // Another user created a channel — add it to our sidebar
    if (!m_channels.contains(channel)) {
        m_channels.append(channel);
        addChannelToList(channel);
    }
}

void MainWindow::onMessageHistoryReceived(const QString &channel,
                                          const QVector<Message> &messages)
{
    // Replace local message store with server history
    m_messages[channel] = messages;

    // If this is the channel we're currently looking at, reload the view
    if (channel == m_activeChannel) {
        m_chatView->loadChannel(m_activeChannel, m_messages[m_activeChannel]);
    }
}

// ── Channel management ───────────────────────────────────────────────────────

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

    // Persist the channel on the server and notify other users
    m_ws->sendChannelCreate(name);

    // Select the new channel
    int lastIndex = m_channelTree->topLevelItemCount() - 1;
    QTreeWidgetItem *newItem = m_channelTree->topLevelItem(lastIndex);

    if (newItem) {
        m_channelTree->setCurrentItem(newItem);
        onTreeItemClicked(newItem, 0);
    }
}

void MainWindow::onRemoveCurrentChannel()
{
    QTreeWidgetItem *currentItem = m_channelTree->currentItem();
    if (!currentItem) return;

    QString data = currentItem->data(0, Qt::UserRole).toString();

    if (data.startsWith("CHANNEL:")) {
        QString name = data.mid(8);
        m_channels.removeOne(name);
        m_messages.remove(name);
        delete currentItem;

        if (!m_channels.isEmpty()) {
            QTreeWidgetItem *topItem = m_channelTree->topLevelItem(0);
            m_channelTree->setCurrentItem(topItem);
            onTreeItemClicked(topItem, 0);
        } else {
            m_activeChannel.clear();
        }
    }
    else if (data.startsWith("FILE:")) {
        delete currentItem;
    }
}

void MainWindow::onTreeItemClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);
    if (!item) return;

    QString data = item->data(0, Qt::UserRole).toString();

    if (data.startsWith("CHANNEL:")) {
        m_activeChannel = data.mid(8);

        // Load whatever we have locally (may be empty on first join)
        m_chatView->loadChannel(m_activeChannel, m_messages[m_activeChannel]);

        // Join on the server — this triggers message_history + file list
        m_ws->joinChannel(m_activeChannel);
    }
}

// ── Chat ─────────────────────────────────────────────────────────────────────

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

// ── Connection state ─────────────────────────────────────────────────────────

void MainWindow::onWsConnected()
{
    m_statusDot->setStyleSheet("background: #4CAF50; border-radius: 4px;");
    m_chatView->setInputEnabled(true);
    statusBar()->showMessage("Connected  —  " + m_myName);

    // Re-join the active channel so we get fresh history + files
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

// ── File sync ────────────────────────────────────────────────────────────────

void MainWindow::onLocalFileChanged(const QString &filename, int position, int length,
                                    const QString &text, bool isAddition)
{
    if (m_activeChannel.isEmpty()) return;
    m_ws->sendFileEdit(m_activeChannel, filename, position, length, text, isAddition);
}

void MainWindow::onRemoteFileEdit(const QString &channel, const QString &filename,
                                  int position, int length, const QString &text,
                                  bool isAddition)
{
    if (channel != m_activeChannel) return;
    if (filename != m_currentFile) return;

    m_editor->applyRemoteEdit(position, length, text, isAddition);
}

void MainWindow::onFileUploadReceived(const QString &channel, const QString &filename,
                                      const QByteArray &content)
{
    QString localDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation)
    + "/NoteFlow/" + channel;
    QDir().mkpath(localDir);
    QString localPath = localDir + "/" + filename;

    QFile file(localPath);
    if (!file.open(QIODevice::WriteOnly)) return;
    file.write(content);
    file.close();

    for (int i = 0; i < m_channelTree->topLevelItemCount(); ++i) {
        QTreeWidgetItem *channelItem = m_channelTree->topLevelItem(i);
        QString data = channelItem->data(0, Qt::UserRole).toString();

        if (data == "CHANNEL:" + channel) {
            // Avoid duplicates
            bool alreadyExists = false;
            for (int j = 0; j < channelItem->childCount(); ++j) {
                QString childData = channelItem->child(j)->data(0, Qt::UserRole).toString();
                if (childData == "FILE:" + localPath) {
                    alreadyExists = true;
                    break;
                }
            }
            if (alreadyExists) break;

            auto *fileItem = new QTreeWidgetItem(channelItem);
            fileItem->setData(0, Qt::UserRole, "FILE:" + localPath);
            auto *fileWidget = new fileItemWidget(m_channelTree, localPath);
            m_channelTree->setItemWidget(fileItem, 0, fileWidget);
            channelItem->setExpanded(true);

            connect(fileWidget, &fileItemWidget::fileSelected,
                    this, [this](const QString &path) {
                        m_editor->loadFile(path);
                        m_currentFile = QFileInfo(path).fileName();
                    });
            break;
        }
    }
}

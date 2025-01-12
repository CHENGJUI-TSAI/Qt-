﻿#include "NetworkGame.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QMessageBox>
#include <QInputDialog>
#include <QHostAddress>
#include <QPainter>
#include <QMouseEvent>

static const int BOARD_SIZE = 15;
static const int CELL_SIZE = 40;

NetworkGame::NetworkGame(QWidget *parent)
    : QMainWindow(parent),
    tcpServer(nullptr),
    tcpSocket(nullptr),
    isHost(false),
    currentPlayer(1),
    board(BOARD_SIZE, QVector<int>(BOARD_SIZE, 0))
{
    setWindowTitle("Network Game");

    if (!boardImage.load(":/icons/images/board.png")) {
        QMessageBox::warning(this, "Error", "Failed to load board image.");
    }

    statusLabel = new QLabel("等待操作...", this);
    hostButton = new QPushButton("建立房間", this);
    joinButton = new QPushButton("加入房間", this);
    chatView = new QTextEdit(this);
    chatView->setReadOnly(true);
    chatInput = new QTextEdit(this);
    sendButton = new QPushButton("發送", this);
    undoButton = new QPushButton("悔棋", this);

    QWidget *mainWidget = new QWidget(this);
    setCentralWidget(mainWidget);
    QHBoxLayout *mainLayout = new QHBoxLayout(mainWidget);

    QWidget *chessboardArea = new QWidget(this);
    chessboardArea->setFixedSize(BOARD_SIZE * CELL_SIZE, BOARD_SIZE * CELL_SIZE);

    QVBoxLayout *rightLayout = new QVBoxLayout();
    rightLayout->addWidget(statusLabel);
    rightLayout->addWidget(hostButton);
    rightLayout->addWidget(joinButton);
    rightLayout->addWidget(chatView);
    rightLayout->addWidget(chatInput);
    rightLayout->addWidget(sendButton);
    rightLayout->addWidget(undoButton);

    mainLayout->addWidget(chessboardArea);
    mainLayout->addLayout(rightLayout);

    setFixedSize(BOARD_SIZE * CELL_SIZE + 300, BOARD_SIZE * CELL_SIZE + 20);

    connect(hostButton, &QPushButton::clicked, this, &NetworkGame::onHostButtonClicked);
    connect(joinButton, &QPushButton::clicked, this, &NetworkGame::onJoinButtonClicked);
    connect(sendButton, &QPushButton::clicked, this, &NetworkGame::onSendMessage);
    connect(undoButton, &QPushButton::clicked, this, &NetworkGame::onUndoButtonClicked);
}


void NetworkGame::keyPressEvent(QKeyEvent *event)
{
    if ((event->modifiers() & Qt::ControlModifier) && event->key() == Qt::Key_S && chatInput->hasFocus()) {
        // 如果焦點在 chatInput 上，按下 Ctrl+S 時觸發發送訊息功能
        qDebug() << "Ctrl+S pressed in chatInput, triggering send message...";
        onSendMessage();  // 呼叫發送訊息的函數
        return;
    }

    if ((event->modifiers() & Qt::ControlModifier) && event->key() == Qt::Key_Z) {
        // Ctrl+Z 按鍵觸發悔棋功能
        qDebug() << "Ctrl+Z pressed, triggering undo...";
        onUndoButtonClicked();  // 呼叫悔棋功能
        return;
    }

    // 確保其他按鍵事件正常傳遞
    QMainWindow::keyPressEvent(event);
}




NetworkGame::~NetworkGame()
{
    if (tcpServer) {
        tcpServer->close();
        delete tcpServer;
    }
    if (tcpSocket) {
        tcpSocket->close();
        delete tcpSocket;
    }
}

void NetworkGame::paintEvent(QPaintEvent *event)
{
    qDebug() << "Updating chessboard";  // 確認畫面是否刷新
    QPainter painter(this);

    if (!boardImage.isNull()) {
        painter.drawPixmap(0, 0, BOARD_SIZE * CELL_SIZE, BOARD_SIZE * CELL_SIZE, boardImage);
    } else {
        painter.fillRect(0, 0, BOARD_SIZE * CELL_SIZE, BOARD_SIZE * CELL_SIZE, QColor(220, 180, 140));
    }

    painter.setPen(Qt::black);
    for (int i = 0; i <= BOARD_SIZE; ++i) {
        painter.drawLine(CELL_SIZE / 2, CELL_SIZE / 2 + i * CELL_SIZE,
                         CELL_SIZE / 2 + BOARD_SIZE * CELL_SIZE, CELL_SIZE / 2 + i * CELL_SIZE);
        painter.drawLine(CELL_SIZE / 2 + i * CELL_SIZE, CELL_SIZE / 2,
                         CELL_SIZE / 2 + i * CELL_SIZE, CELL_SIZE / 2 + BOARD_SIZE * CELL_SIZE);
    }

    drawStarPoints(painter);

    for (int y = 0; y < BOARD_SIZE; ++y) {
        for (int x = 0; x < BOARD_SIZE; ++x) {
            if (board[y][x] == 1) {
                painter.setBrush(Qt::black);
            } else if (board[y][x] == 2) {
                painter.setBrush(Qt::white);
            } else {
                continue;
            }
            painter.drawEllipse(x * CELL_SIZE + CELL_SIZE / 2 - 15,
                                y * CELL_SIZE + CELL_SIZE / 2 - 15, 30, 30);
        }
    }
}


void NetworkGame::mousePressEvent(QMouseEvent *event)
{
    if ((!isHost && currentPlayer != 1) || (isHost && currentPlayer != 2)) return;

    int x = (event->x() - CELL_SIZE / 2) / CELL_SIZE;
    int y = (event->y() - CELL_SIZE / 2) / CELL_SIZE;

    if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE || board[y][x] != 0) {
        return;
    }

    // 更新本地棋盤
    board[y][x] = currentPlayer;
    moveHistory.push({x, y});

    // 發送座標給對方
    if (tcpSocket && tcpSocket->state() == QAbstractSocket::ConnectedState) {
        QString move = QString("MOVE:%1,%2").arg(x).arg(y);
        tcpSocket->write(move.toUtf8() + "\n");
        qDebug() << "Sent move:" << move;  // 確認座標發送
    }

    // 切換當前玩家
    currentPlayer = 3 - currentPlayer;

    // 更新畫面
    update();

    // 檢查勝負
    if (checkWin(x, y)) {
        QMessageBox::information(this, "遊戲結束", board[y][x] == 1 ? "黑棋勝利！" : "白棋勝利！");
        resetGame();
        return;
    }
}


void NetworkGame::onHostButtonClicked()
{
    if (tcpServer) {
        QMessageBox::information(this, "提示", "伺服器已經啟動！");
        return;
    }

    tcpServer = new QTcpServer(this);
    connect(tcpServer, &QTcpServer::newConnection, this, &NetworkGame::onNewConnection);

    if (tcpServer->listen(QHostAddress::Any, 12345)) {
        QMessageBox::information(this, "伺服器啟動", "伺服器已啟動，等待連線...");
        statusLabel->setText("伺服器已啟動，等待連線...");
        isHost = true;
    } else {
        QMessageBox::critical(this, "錯誤", "伺服器啟動失敗：" + tcpServer->errorString());
        delete tcpServer;
        tcpServer = nullptr;
    }
}

void NetworkGame::onJoinButtonClicked()
{
    if (tcpSocket) {
        QMessageBox::information(this, "提示", "您已經連接到伺服器！");
        return;
    }

    QString serverIP = QInputDialog::getText(this, "加入房間", "輸入伺服器 IP：", QLineEdit::Normal, "127.0.0.1");
    if (serverIP.isEmpty()) {
        return;
    }

    bool ok;
    int port = QInputDialog::getInt(this, "加入房間", "輸入伺服器端口（默認 12345）：", 12345, 1024, 65535, 1, &ok);

    if (!ok) {
        QMessageBox::information(this, "提示", "加入房間取消。");
        return;
    }

    tcpSocket = new QTcpSocket(this);
    connect(tcpSocket, &QTcpSocket::readyRead, this, &NetworkGame::onReadyRead);
    connect(tcpSocket, &QTcpSocket::disconnected, this, &NetworkGame::onDisconnected);

    tcpSocket->connectToHost(QHostAddress(serverIP), port);
    if (tcpSocket->waitForConnected(5000)) {
        QMessageBox::information(this, "連線成功", "成功連接到伺服器！");
        statusLabel->setText("已連接到伺服器！");
    } else {
        QMessageBox::critical(this, "錯誤", "無法連接到伺服器：" + tcpSocket->errorString());
        delete tcpSocket;
        tcpSocket = nullptr;
    }
}


void NetworkGame::onSendMessage()
{
    if (!tcpSocket || tcpSocket->state() != QAbstractSocket::ConnectedState) {
        QMessageBox::warning(this, "錯誤", "尚未連接到伺服器！");
        return;
    }

    QString message = chatInput->toPlainText().trimmed();
    if (message.isEmpty()) {
        QMessageBox::warning(this, "提示", "訊息不可為空！");
        return;
    }

    // 發送訊息到對方
    tcpSocket->write(message.toUtf8() + "\n");

    // 顯示在本地聊天室
    chatView->append("我: " + message);
    chatInput->clear();
}


void NetworkGame::onUndoButtonClicked()
{
    if (moveHistory.isEmpty()) {
        QMessageBox::information(this, "悔棋", "沒有步數可以悔棋！");
        return;
    }

    // 檢查是否是對應的玩家
    QPair<int, int> lastMove = moveHistory.top();
    if ((isHost && board[lastMove.second][lastMove.first] != 2) ||
        (!isHost && board[lastMove.second][lastMove.first] != 1)) {
        QMessageBox::warning(this, "悔棋失敗", "您只能撤回您的棋子！");
        return;
    }

    // 更新本地棋盤
    moveHistory.pop();
    board[lastMove.second][lastMove.first] = 0;
    currentPlayer = 3 - currentPlayer;
    update();

    // 發送悔棋指令
    if (tcpSocket && tcpSocket->state() == QAbstractSocket::ConnectedState) {
        QString undoMessage = QString("UNDO:%1,%2").arg(lastMove.first).arg(lastMove.second);
        tcpSocket->write(undoMessage.toUtf8() + "\n");
        qDebug() << "Sent undo message:" << undoMessage;
    }
}



void NetworkGame::onNewConnection()
{
    if (!tcpServer) return;

    tcpSocket = tcpServer->nextPendingConnection();
    connect(tcpSocket, &QTcpSocket::readyRead, this, &NetworkGame::onReadyRead);
    connect(tcpSocket, &QTcpSocket::disconnected, this, &NetworkGame::onDisconnected);

    QMessageBox::information(this, "新連線", "客戶端已連線！");
    statusLabel->setText("客戶端已連線！");
}

void NetworkGame::onReadyRead()
{
    if (!tcpSocket) return;

    while (tcpSocket->canReadLine()) {
        QString message = QString::fromUtf8(tcpSocket->readLine()).trimmed();
        qDebug() << "Received message:" << message;

        if (message.startsWith("MOVE:")) {
            // 更新對方下的棋
            QStringList parts = message.mid(5).split(",");
            if (parts.size() == 2) {
                int x = parts[0].toInt();
                int y = parts[1].toInt();
                board[y][x] = currentPlayer;
                currentPlayer = 3 - currentPlayer;
                update();

                if (checkWin(x, y)) {
                    QMessageBox::information(this, "遊戲結束", board[y][x] == 1 ? "黑棋勝利！" : "白棋勝利！");
                    resetGame();
                }
            }
        } else if (message.startsWith("UNDO:")) {
            // 解析悔棋的座標
            QStringList parts = message.mid(5).split(",");
            if (parts.size() == 2) {
                int x = parts[0].toInt();
                int y = parts[1].toInt();
                board[y][x] = 0;
                currentPlayer = 3 - currentPlayer;
                update();
                qDebug() << "Processed undo for:" << x << y;
            }
        } else {
            chatView->append("對方: " + message);
        }
    }
}





void NetworkGame::onDisconnected()
{
    QMessageBox::information(this, "連線中斷", "與對方的連線已中斷！");
    statusLabel->setText("未連線");
    if (tcpSocket) {
        tcpSocket->deleteLater();
        tcpSocket = nullptr;
    }
}

bool NetworkGame::checkWin(int x, int y)
{
    static const int directions[4][2] = {
        {1, 0}, {0, 1}, {1, 1}, {1, -1}
    };

    int stone = board[y][x];
    for (auto &dir : directions) {
        int count = 1;
        for (int i = 1; i < 5; ++i) {
            int nx = x + i * dir[0], ny = y + i * dir[1];
            if (nx < 0 || nx >= BOARD_SIZE || ny < 0 || ny >= BOARD_SIZE || board[ny][nx] != stone) break;
            ++count;
        }
        for (int i = 1; i < 5; ++i) {
            int nx = x - i * dir[0], ny = y - i * dir[1];
            if (nx < 0 || nx >= BOARD_SIZE || ny < 0 || ny >= BOARD_SIZE || board[ny][nx] != stone) break;
            ++count;
        }
        if (count >= 5) return true;
    }
    return false;
}

void NetworkGame::resetGame()
{
    board.fill(QVector<int>(BOARD_SIZE, 0));
    moveHistory.clear();
    currentPlayer = 1;
    update();
}

void NetworkGame::drawStarPoints(QPainter &painter)
{
    painter.setBrush(Qt::black);
    QVector<QPair<int, int>> starPoints = {
        {3, 3}, {3, 11}, {11, 3}, {11, 11}, {7, 7}
    };

    for (const auto &point : starPoints) {
        painter.drawEllipse(point.first * CELL_SIZE + CELL_SIZE / 2 - 5,
                            point.second * CELL_SIZE + CELL_SIZE / 2 - 5, 10, 10);
    }
}

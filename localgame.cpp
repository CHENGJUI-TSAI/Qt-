#include "LocalGame.h"
#include <QPainter>
#include <QMouseEvent>
#include <QMessageBox>
#include <QKeyEvent>

LocalGame::LocalGame(QWidget *parent)
    : QWidget(parent),
    currentPlayer(1),
    undoButton(nullptr),
    currentPlayerLabel(nullptr)
{
    setFixedSize(BOARD_SIZE * CELL_SIZE + 120, BOARD_SIZE * CELL_SIZE);
    board = QVector<QVector<int>>(BOARD_SIZE, QVector<int>(BOARD_SIZE, 0));
    boardImage.load(":/icons/images/board.png");

    currentPlayerLabel = new QLabel(this);
    currentPlayerLabel->setGeometry(BOARD_SIZE * CELL_SIZE + 10, 20, 100, 30);
    currentPlayerLabel->setAlignment(Qt::AlignCenter);

    undoButton = new QPushButton(tr("悔棋"), this);
    undoButton->setGeometry(BOARD_SIZE * CELL_SIZE + 10, 60, 100, 30);
    connect(undoButton, &QPushButton::clicked, this, &LocalGame::undoLastMove);

    updateCurrentPlayerLabel();
}

void LocalGame::startGame()
{
    board.fill(QVector<int>(BOARD_SIZE, 0));
    currentPlayer = 1;
    moveHistory.clear();
    update();
    updateCurrentPlayerLabel();
}

void LocalGame::paintEvent(QPaintEvent * /*event*/)
{
    QPainter painter(this);
    QPixmap scaledBoard = boardImage.scaled(
        BOARD_SIZE * CELL_SIZE,
        BOARD_SIZE * CELL_SIZE,
        Qt::IgnoreAspectRatio,
        Qt::SmoothTransformation
        );

    painter.fillRect(rect(), QColor(60, 60, 60));
    painter.drawPixmap(0, 0, scaledBoard);

    for (int row = 0; row < BOARD_SIZE; ++row) {
        for (int col = 0; col < BOARD_SIZE; ++col) {
            int stone = board[row][col];
            if (stone == 0) continue;

            if (stone == 1) {
                painter.setBrush(Qt::black);
            } else if (stone == 2) {
                painter.setBrush(Qt::white);
            }

            painter.drawEllipse(
                col * CELL_SIZE + CELL_SIZE / 2 - 12,
                row * CELL_SIZE + CELL_SIZE / 2 - 12,
                24, 24
                );
        }
    }
}

void LocalGame::mousePressEvent(QMouseEvent *event)
{
    if (event->x() >= BOARD_SIZE * CELL_SIZE) {
        return;
    }

    int col = (event->x() - CELL_SIZE / 2) / CELL_SIZE;
    int row = (event->y() - CELL_SIZE / 2) / CELL_SIZE;

    if (col >= 0 && col < BOARD_SIZE &&
        row >= 0 && row < BOARD_SIZE &&
        board[row][col] == 0)
    {
        board[row][col] = currentPlayer;
        moveHistory.push({col, row});

        if (checkWin(col, row)) {
            QMessageBox::information(
                this, tr("遊戲結束"),
                currentPlayer == 1 ? tr("黑棋獲勝！") : tr("白棋獲勝！")
                );
            startGame();
            return;
        }

        currentPlayer = 3 - currentPlayer;
        update();
        updateCurrentPlayerLabel();
    }
}

bool LocalGame::checkWin(int x, int y)
{
    const int directions[4][2] = {
        {1, 0},//水平
        {0, 1},//垂直
        {1, 1},//斜向{1, 1}
        {1, -1}//斜向{1, -1}
    };

    for (auto &dir : directions) {//正向
        int count = 1;

        for (int i = 1; i < 5; ++i) {
            int nx = x + i * dir[0];
            int ny = y + i * dir[1];
            if (nx >= 0 && nx < BOARD_SIZE &&
                ny >= 0 && ny < BOARD_SIZE &&
                board[ny][nx] == currentPlayer)
            {
                ++count;
            } else {
                break;
            }
        }

        for (int i = 1; i < 5; ++i) {//反向
            int nx = x - i * dir[0];
            int ny = y - i * dir[1];
            if (nx >= 0 && nx < BOARD_SIZE &&
                ny >= 0 && ny < BOARD_SIZE &&
                board[ny][nx] == currentPlayer)
            {
                ++count;
            } else {
                break;
            }
        }

        if (count >= 5) {
            return true;
        }
    }
    return false;
}

void LocalGame::undoLastMove()
{
    if (!moveHistory.isEmpty()) {
        auto lastMove = moveHistory.pop();
        board[lastMove.second][lastMove.first] = 0;
        currentPlayer = 3 - currentPlayer;
        update();
        updateCurrentPlayerLabel();
    } else {
        QMessageBox::information(this, tr("無法悔棋"), tr("沒有步驟可悔棋！"));
    }
}

void LocalGame::updateCurrentPlayerLabel()
{
    if (!currentPlayerLabel) return;
    currentPlayerLabel->setText(
        currentPlayer == 1 ? tr("黑棋回合") : tr("白棋回合")
        );
}

void LocalGame::keyPressEvent(QKeyEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier && event->key() == Qt::Key_Z) {
        qDebug() << "Ctrl+Z pressed, triggering undo...";
        undoLastMove();
        return;
    }

    QWidget::keyPressEvent(event);
}

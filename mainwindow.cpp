#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    initializeBoard();

    // 連接右側按鈕信號與槽函數
    connect(ui->btn_new_game, &QPushButton::clicked, this, [this]() {
        // 新遊戲邏輯
        initializeBoard();
    });
    connect(ui->btn_load_game, &QPushButton::clicked, this, []() {
        // 載入遊戲邏輯
    });
    connect(ui->btn_save_game, &QPushButton::clicked, this, []() {
        // 儲存遊戲邏輯
    });
    connect(ui->btn_exit_game, &QPushButton::clicked, this, []() {
        QApplication::quit();
    });
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::initializeBoard() {
    // 清空舊棋盤按鈕
    for (auto& row : boardButtons) {
        for (auto* button : row) {
            delete button;
        }
    }
    boardButtons.clear();

    // 動態生成 15x15 棋盤按鈕
    QGridLayout* layout = new QGridLayout(ui->centralwidget);
    for (int i = 0; i < 15; ++i) {
        std::vector<QPushButton*> row;
        for (int j = 0; j < 15; ++j) {
            QPushButton* button = new QPushButton(this);
            button->setFixedSize(40, 40);
            layout->addWidget(button, i, j);
            connect(button, &QPushButton::clicked, this, &MainWindow::handleButtonClick);
            row.push_back(button);
        }
        boardButtons.push_back(row);
    }
}

void MainWindow::handleButtonClick() {
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (button) {
        button->setText("●"); // 模擬落子
        button->setEnabled(false); // 禁用按鈕
    }
}

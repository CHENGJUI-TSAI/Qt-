#include "Qtproject.h"
#include "ui_Qtproject.h"
#include "LocalGame.h"
#include "AIvsPlayer.h"
#include "NetworkGame.h"

Qtproject::Qtproject(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Qtproject)
{
    ui->setupUi(this);

    // 設置背景圖片
    QPixmap pixmap(":/icons/images/background.png");
    QPalette palette;
    palette.setBrush(backgroundRole(), QBrush(pixmap.scaled(this->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));
    setPalette(palette);

    // 設置按鈕圖標
    QIcon buttonIcon(":/icons/images/button1.png");
    ui->pushButton_1->setIcon(buttonIcon);
    ui->pushButton_1->setIconSize(QSize(200, 120));

    QIcon buttonIcon2(":/icons/images/button2.png");
    ui->pushButton_2->setIcon(buttonIcon2);
    ui->pushButton_2->setIconSize(QSize(200, 120));

    QIcon buttonIcon3(":/icons/images/button3.png");
    ui->pushButton_3->setIcon(buttonIcon3);
    ui->pushButton_3->setIconSize(QSize(200, 120));

    // 連接按鈕事件
    connect(ui->pushButton_1, &QPushButton::clicked, this, &Qtproject::onButton1Clicked);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &Qtproject::onButton2Clicked);
    connect(ui->pushButton_3, &QPushButton::clicked, this, &Qtproject::onButton3Clicked);
}

Qtproject::~Qtproject()
{
    delete ui;
}

void Qtproject::onButton1Clicked()
{
    LocalGame localGame;
    localGame.startGame();
}

void Qtproject::onButton2Clicked()
{
    AIvsPlayer aiGame;
    aiGame.startGame();
}

void Qtproject::onButton3Clicked()
{
    NetworkGame networkGame;
    networkGame.startGame();
}

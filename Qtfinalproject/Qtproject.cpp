#include "Qtproject.h"
#include "ui_Qtproject.h"

Qtproject::Qtproject(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Qtproject)
{
    ui->setupUi(this);

    // 設置背景圖片並讓我調整大小
    QPixmap pixmap(":/icons/images/background.png");
    QPalette palette;
    palette.setBrush(backgroundRole(), QBrush(pixmap.scaled(this->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));
    setPalette(palette);

    // 設置按鈕圖標並調整大小
    QIcon buttonIcon(":/icons/images/button1.png");
    ui->pushButton_1->setIcon(buttonIcon);
    ui->pushButton_1->setIconSize(QSize(200, 120));

    QIcon buttonIcon2(":/icons/images/button2.png");
    ui->pushButton_2->setIcon(buttonIcon2);
    ui->pushButton_2->setIconSize(QSize(200, 120));

    QIcon buttonIcon3(":/icons/images/button3.png");
    ui->pushButton_3->setIcon(buttonIcon3);
    ui->pushButton_3->setIconSize(QSize(200, 120));

    // 連接按鈕點擊事件與相應的槽函數
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
    // 處理 button1 點擊事件
    qDebug("本地對戰被點擊!");
}

void Qtproject::onButton2Clicked()
{
    // 處理 button2 點擊事件
    qDebug("人機對戰被點擊!");
}

void Qtproject::onButton3Clicked()
{
    // 處理 button3 點擊事件
    qDebug("連線對戰被點擊!");
}

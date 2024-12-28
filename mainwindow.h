#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <vector>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT


public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void handleButtonClick(); // 處理棋盤按鈕點擊

private:
    Ui::MainWindow *ui;
    std::vector<std::vector<QPushButton*>> boardButtons; // 儲存棋盤按鈕
    void initializeBoard(); // 初始化棋盤
};

#endif // MAINWINDOW_H

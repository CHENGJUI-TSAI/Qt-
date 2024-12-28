#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QRadioButton>
#include <QPushButton>
#include "ui_SettingsDialog.h"  // 由 Qt Designer 自動生成的 UI 類

class SettingsDialog : public QDialog, private Ui::SettingsDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);

    QString getBoardSize();  // 返回選擇的棋盤大小
    QString getGameMode();   // 返回選擇的對戰模式

private slots:
    void on_buttonOK_clicked();    // 確定按鈕
    void on_buttonCancel_clicked(); // 取消按鈕
};

#endif // SETTINGSDIALOG_H

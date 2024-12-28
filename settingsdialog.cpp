#include "SettingsDialog.h"
#include <QMessageBox>

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);  // 設置 UI

    // 設置棋盤大小選項
    comboBoxBoardSize->addItem("15x15");
    comboBoxBoardSize->addItem("19x19");
    comboBoxBoardSize->addItem("13x13");

    // 設置對戰模式的默認選擇
    radioButtonPlayerVsPlayer->setChecked(true);

    // 連接按鈕點擊信號
    connect(buttonOK, &QPushButton::clicked, this, &SettingsDialog::on_buttonOK_clicked);
    connect(buttonCancel, &QPushButton::clicked, this, &SettingsDialog::on_buttonCancel_clicked);
}

QString SettingsDialog::getBoardSize()
{
    return comboBoxBoardSize->currentText();
}

QString SettingsDialog::getGameMode()
{
    if (radioButtonPlayerVsPlayer->isChecked()) {
        return "Player vs Player";
    } else {
        return "Player vs AI";
    }
}

void SettingsDialog::on_buttonOK_clicked()
{
    // 確定選擇後返回主界面
    accept();
}

void SettingsDialog::on_buttonCancel_clicked()
{
    // 取消選擇
    reject();
}

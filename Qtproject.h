#ifndef QTPROJECT_H
#define QTPROJECT_H

#include <QMainWindow>
#include <QPushButton>

namespace Ui {
class Qtproject;
}

class Qtproject : public QMainWindow
{
    Q_OBJECT

public:
    explicit Qtproject(QWidget *parent = nullptr);
    ~Qtproject();

private slots:
    void onButton1Clicked();
    void onButton2Clicked();
    void onButton3Clicked();

private:
    Ui::Qtproject *ui;
};

#endif // QTPROJECT_H

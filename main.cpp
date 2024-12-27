#include "Qtproject.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Qtproject w;
    w.show();
    return a.exec();
}

#ifndef LOCALGAME_H
#define LOCALGAME_H

#include <QObject>
#include <QDebug>

class LocalGame : public QObject
{
    Q_OBJECT

public:
    void startGame();
};

#endif // LOCALGAME_H

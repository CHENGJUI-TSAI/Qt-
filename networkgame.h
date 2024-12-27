#ifndef NETWORKGAME_H
#define NETWORKGAME_H

#include <QObject>
#include <QDebug>

class NetworkGame : public QObject
{
    Q_OBJECT

public:
    void startGame();
};

#endif // NETWORKGAME_H

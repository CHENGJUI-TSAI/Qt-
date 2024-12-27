#ifndef AIVSPLAYER_H
#define AIVSPLAYER_H

#include <QObject>
#include <QDebug>

class AIvsPlayer : public QObject
{
    Q_OBJECT

public:
    void startGame();
};

#endif // AIVSPLAYER_H

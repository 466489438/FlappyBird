#ifndef GAMECONTORLLER_H
#define GAMECONTORLLER_H
#include <QLCDNumber>
#include <QObject>
#include "mainwindow.h"

class roadItem;
class bird;
class barrier;

class gameController : public QObject
{
    Q_OBJECT

public:
    explicit gameController(QGraphicsScene &Scene, QObject *parent = 0);
    void initReadyWidget();

signals:
    void close();

public slots:
    void readyWidgetMove();
    void startGame();
    void gameLoop();
    void gameOver();
    void add_Score();

protected:
    bool eventFilter(QObject *object, QEvent *event);

private:
    bool isBegin;
    int current_Score;
    int distance;
    QWidget *readyWidget;
    QWidget *scoreWidget;
    QLabel *score;
	QMediaPlayer *media;
    QGraphicsScene *scene;
    QPropertyAnimation *readyAnimation;
    roadItem *roadOne;
    roadItem *roadTwo;
    bird *flyBird;
    QList<barrier *> barrierList;
	QPushButton *restartButton;
    QPushButton *quitButton;
	QLCDNumber *lcdScore;//显示分数
    QTimer *gameLoopTimer;
	QPixmap pixmapBkgnd;//游戏背景
};

#endif // GAMECONTORLLER_H

#ifndef BIRD_H
#define BIRD_H

#include <QGraphicsObject>
#include "mainwindow.h"

class bird : public QGraphicsObject
{
    Q_OBJECT

public:
    bird(QGraphicsObject *parent = 0);
    void keyPressEvent(QKeyEvent *event);
	//void mousePressEvent(QMouseEvent *event);
    void riseAndfall();
    void birdUp();
    bool checkCollide();
	void stopMove();
	void reset();
protected:
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	
signals:
    void adjustTimer();

protected slots:
    void setChangeAnimation();
    

private:
    int status;
    int statusChange;
    bool changeAnimation;
    bool is_down;
    //bool can_up;
    QMediaPlayer *media;
    QPropertyAnimation *animation;
};

#endif // BIRD_H

#include "roaditem.h"

roadItem::roadItem(int ID, QGraphicsObject *parent) : QGraphicsObject(parent)
{
    id = ID;//标记是哪一个道路
}
// boundingRect paint不再多说-0-。。。。
QRectF roadItem::boundingRect() const
{
    if (1 == id)
        return QRectF(0, ROAD_YVALUE, WIDTH, ROAD_HEIGHT);
    else
        return QRectF(WIDTH - 2, ROAD_YVALUE, WIDTH, ROAD_HEIGHT);
}

void roadItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    if (1 == id)
    {
        painter->save();
        painter->drawImage(QRectF(0, ROAD_YVALUE, WIDTH, ROAD_HEIGHT), QImage(":/resource/Resource/roadMove.png"));
        painter->restore();
    }
    else
    {
        painter->save();
        painter->drawImage(QRectF(WIDTH - 2, ROAD_YVALUE, WIDTH, ROAD_HEIGHT), QImage(":/resource/Resource/roadMove.png"));
        painter->restore();
    }
}

void roadItem::beginMove()
{
    animation = new QPropertyAnimation(this, "pos");

    if (id == 1)
    {
        animation->setDuration(ROAD_TIME);//道路1和道路2的初试位置相差一个视图的宽度,所以移动时间和距离不同
        animation->setStartValue(pos());
        animation->setEndValue(QPoint(pos().x()-WIDTH, pos().y()));
        animation->start();
        connect(animation, SIGNAL(finished()), this, SLOT(rebegin()));//第一次动画结束后重新定位和动画
    }
    else
    {
        animation->setDuration(ROAD_TIME * 2);
        animation->setStartValue(QPoint(pos().x() - 2, pos().y()));
        animation->setEndValue(QPoint(pos().x() - WIDTH * 2, pos().y()));
        animation->setLoopCount(-1);//道路2直接设置无限循环
        animation->start();
    }
}

void roadItem::rebegin()
{
    disconnect(animation, SIGNAL(finished()), this, SLOT(rebegin()));//取消关联
    animation->setDuration(ROAD_TIME * 2);//设置和道路2一样的移动方式
    animation->setStartValue(QPoint(pos().x() + WIDTH * 2 - 7, pos().y()));
    animation->setEndValue(pos());
    animation->setLoopCount(-1);//设置无限循环
    animation->start();
}

void roadItem::stopMove()
{
    setPos(0, 0);//游戏结束后重置位置
    animation->stop();
}


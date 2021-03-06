#include "barrier.h"

barrier::barrier()
{
    is_addScore = false;    //标记是否增加分数
    QTime time = QTime::currentTime();//获取当前时间
    qsrand(time.msec() + time.second() * 1000);//设置随机数种子
    height = qrand() % 161+ 50;//产生随机数设置该障碍物上管道的高度
    height2 = 274 - height;//计算下管道的高度
    width = 40;//设定管道的宽度
    beginMove();//管道开始移动
}

QRectF barrier::boundingRect() const//返回改管道所在的矩形
{
    return QRectF(WIDTH, 0, width, HEIGHT);
}

void barrier::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{//绘制管道
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->save();
    //绘制上管道
    painter->drawImage(QRectF(WIDTH, 0, width, height), QImage(":/resource/Resource/pipe1.png"));
    //绘制下管道
    painter->drawImage(QRectF(WIDTH, height + 110, width, height2), QImage(":/resource/Resource/pipe2.png").scaled(width, height2));
    painter->restore();

    if (!is_addScore && pos().x() < -190)//如果管道的坐标小于190则判定小鸟成功通过该管道
    {
        is_addScore = true;//标记增加分数,一个管道只能增加一次分数.
        emit add_Score();//发送增加分数的信号
    }
}

QPainterPath barrier::shape() const
{
    QPainterPath path;
    //碰撞检测需要调用该函数,通过shape判定是否有碰撞,因为在上下管道之间有小鸟可以飞过的通道,所以要重写该函数
    //不重写该函数则以boundingRect构成的矩形为默认形状
    path.addRect(QRectF(WIDTH, 0, width, height));//添加上管道的矩形
    path.addRect(QRectF(WIDTH, height + 125, width, height2));//添加下管道的矩形 并相应减小高度
    //小鸟下方大约有15相熟空白
    return path;
}

void barrier::beginMove()
{
    animation = new QPropertyAnimation(this, "pos");//设置动画移动
    animation->setDuration(BARRIER_TIME);
    animation->setStartValue(pos());
    animation->setEndValue(QPoint(pos().x() - WIDTH - 50, pos().y()));
    animation->start();
}

void barrier::stopMove()
{
	animation->stop();
}

barrier::~barrier()
{
}

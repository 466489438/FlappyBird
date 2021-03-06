#include "bird.h"

bird::bird(QGraphicsObject *parent) : QGraphicsObject(parent)
{
    changeAnimation = true;//标记是否改变动画 也就是在上飞的动画和降落的动画之间切换
    animation = new QPropertyAnimation(this, "pos");
    media = new QMediaPlayer;//设置背景飞的时候的背景音乐
    media->setMedia(QUrl("qrc:/resource/Music/fly.mp3"));
    media->setVolume(60);
    setPos(QPoint(0, 0));
}

QRectF bird::boundingRect() const
{
    return QRectF(175, 215, BIRD_SIZE, BIRD_SIZE);//同barrier的boundRect函数
}

void bird::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{//原理基本同barrier的paint函数
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->save();
  
    painter->drawImage(boundingRect(), QImage(":/resource/Resource/bird1.png"));//载入飞翔小鸟图
	
    painter->restore();
}

void bird::keyPressEvent(QKeyEvent *event)
{//处理键盘事件,如果按键是空格  则调用birdUp函数
    if (event->key() == Qt::Key_Space)
    {
        birdUp();
    }
}
// void bird::mousePressEvent(QMouseEvent *event)
// {//处理鼠标事件,如果按键是左键  则调用birdUp函数
// 	if (event->button() == Qt::LeftButton)
// 	{
// 		birdUp();
// 	}
// }
void bird::birdUp()
{
        animation->stop();//停止当前的动画
        changeAnimation = false;//设置为不掉落
       
        if (this->pos().y() > BIRD_UPBOUND)//如果纵坐标大于上飞的距离,则上升上飞的距离
        {
            animation->setDuration(BIRD_UPTIME);
            animation->setStartValue(pos());
            animation->setEndValue(QPoint(pos().x(), pos().y() - BIRD_UPDISTANCE));
        }
        else//否则飞到顶端
        {
            animation->setDuration(BIRD_UPTIME);
            animation->setStartValue(pos());
            animation->setEndValue(QPoint(pos().x(), -HEIGHT / 2));
        }
        animation->setEasingCurve(QEasingCurve::OutQuad);//设置动画的宽松曲线
        animation->start();
        media->play();//播放一次扇动翅膀的音频
        connect(animation, SIGNAL(finished()), this, SLOT(setChangeAnimation()));
        //连接信号槽,动画结束后触发setChangeAnimation函数
    
}

void bird::setChangeAnimation()
{
    changeAnimation = true;//标记为true,说明可以设置下降的动画了
   
    emit adjustTimer();//发送调整计时器的信号,即重新计时
    disconnect(animation, SIGNAL(finished()), this, SLOT(setChangeAnimation()));//断开信号槽的连接
}

void bird::riseAndfall()
{
    if (changeAnimation)//在标记为可以下降的时候才重新计算并开始下降的动画
    {
        animation->stop();
        int DValue = HEIGHT - pos().y();//计算小鸟的位置距离游戏下边界的距离
        animation->setDuration(DValue * BIRD_DIVISOR);//距离乘以距离因子得到下降的时间 保证每次速度都一样
        animation->setStartValue(pos());
        animation->setEndValue(QPoint(pos().x(), pos().y() + DValue));
        animation->setEasingCurve(QEasingCurve::InQuad);//设置宽松曲线
        animation->start();
        changeAnimation = false;//标记为false
    }
}

bool bird::checkCollide()
{
    if (collidingItems().isEmpty())//碰撞检测,若有碰撞 则必然是撞道路上或障碍物
        return false;

    media->setMedia(QUrl("qrc:/resource/Music/hit.mp3"));//有碰撞 播放碰撞的音频
    media->play();
    return true;
}

void bird::stopMove()
{
	animation->stop();
}

void bird::reset()
{
    setPos(0, 0);//重置小鸟的位置
    media->setMedia(QUrl("qrc:/resource/Music/fly.mp3"));//重置音频为飞行的音频
    animation->stop();
    changeAnimation = true;
}

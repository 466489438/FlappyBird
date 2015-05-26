#include "gamecontroller.h"
#include<iostream>
gameController::gameController(QGraphicsScene &Scene, QObject *parent) :
QObject(parent)
{
	scene = &Scene;
	current_Score = 0;//初始分数为0
	distance = BARRIER_DISTANCE;//游戏每循环diastance次,进行一次判断是否产生新的管道,避免管道相互距离过紧

	roadOne = new roadItem(1);//道路有两块,循环移动
	roadTwo = new roadItem(2);
	flyBird = new bird();//初始化小鸟
	media = new QMediaPlayer();
	gameLoopTimer = new QTimer(this);//游戏循环的计时器

	isBegin = false;//游戏标记为未开始
	//往场景中加入项目(游戏元素)
	scene->addItem(roadOne);
	scene->addItem(roadTwo);
	scene->addItem(flyBird);
	
	media->setMedia(QUrl("qrc:/resource/Music/past.mp3"));//设置通过管道时播放的音频
	media->setVolume(60);
	initReadyWidget();//初始化选择界面

	connect(gameLoopTimer, SIGNAL(timeout()), this, SLOT(gameLoop()));//关联计时器和游戏循环函数
}

void gameController::initReadyWidget()
{
	scoreBoard = new QLabel();//成绩面板
	quitButton = new QPushButton();//新建按钮
	restartButton = new QPushButton();//新建按钮
	lcdScore = new QLCDNumber(3);//LCD分数显示
	flappyBird = new QLabel();//标题图
	guide = new QLabel();//游戏指导图
	gameOverLabel = new QLabel();//游戏结束图

	high_Score = 0;
	//flappy文字的label
	flappyBird->setPixmap(QPixmap(":resource/Resource/flappybird.png"));
	flappyBird->setStyleSheet("background-color:transparent");
	flappyBird->setGeometry(100, 70, 184, 57);
	//游戏指导的label
	guide->setPixmap(QPixmap(":resource/Resource/guide.png"));
	guide->setStyleSheet("background-color:transparent");
	guide->setGeometry(109, 120, 166, 193);
	//游戏结束的label
	gameOverLabel->setPixmap(QPixmap(":resource/Resource/gameover.png"));
	gameOverLabel->setStyleSheet("background-color:transparent");
	gameOverLabel->setGeometry(64, 70, 256, 57);
	gameOverLabel->setVisible(false);//初始设为不可见
	//成绩面板
	scoreBoard->setGeometry(42, 150, 300, 153);
	scoreBoard->setWindowOpacity(0.9);//透明度10%
	scoreBoard->setStyleSheet("background-color:transparent");
	scoreBoard->setVisible(false);
	//LCD成绩
	lcdScore->setGeometry(50, 50, 80, 50);//设置位置和大小
	lcdScore->setLineWidth(0);
	lcdScore->setStyleSheet("QLCDNumber{color: white;background-color:transparent};");
	//样式表要一次定义完，不然容易乱七八糟的和预期不符
	lcdScore->display(QString("%1").arg(0, 3, 10, QChar('0')));//设置LCD成绩
	lcdScore->setVisible(false);//初始设为不可见
	
	restartButton->setStyleSheet("QPushButton{background-color:transparent;"
		"border-image:url(:resource/Resource/replay.png);}"
		"QPushButton:pressed{margin: 2px 2px 2px 2px;}");//设置样式表
	restartButton->setGeometry(137, 360, 110, 35);//设置按钮的位置和大小
	quitButton->setStyleSheet("QPushButton{background-color:transparent;"
		"border-image:url(:resource/Resource/quit.png);}"
		"QPushButton:pressed{margin: 2px 2px 2px 2px;}");//设置样式表
	quitButton->setGeometry(137, 404, 110, 35);//设置按钮的位置和大小

	scene->addWidget(lcdScore)->setZValue(1);//添加分数显示器到场景中并设置选择界面在场景中Z轴的坐标
	scene->addWidget(restartButton)->setZValue(1);//添加开始按钮到场景中并设置选择界面在场景中Z轴的坐标
	scene->addWidget(quitButton)->setZValue(1);//添加退出按钮到场景中并设置选择界面在场景中Z轴的坐标
	scene->addWidget(scoreBoard)->setZValue(1);//添加成绩板到场景中并设置选择界面在场景中Z轴的坐标
	scene->addWidget(gameOverLabel)->setZValue(1);//添加游戏结束面板到场景中并设置选择界面在场景中Z轴的坐标
	scene->addWidget(flappyBird);//添加标题面板到场景中
	scene->addWidget(guide);//添加游戏指导到场景中
	//scene->addWidget(medal)->setZValue(2);//添加成绩板到场景中并设置选择界面在场景中Z轴的坐标

	connect(quitButton, SIGNAL(clicked()), this, SIGNAL(close()));//关联信号槽
	connect(restartButton, SIGNAL(clicked()), this, SLOT(startGame()));//关联信号槽

	scene->installEventFilter(this);//给场景添加事件过滤器
}

void gameController::startGame()
{
	foreach(barrier *bar, barrierList)//清空管道信息
		delete bar;
	barrierList.clear();//清除列表
	scene->update();//更新场景
	flyBird->reset();//重置小鸟
	current_Score = 0;//分数清零
	lcdScore->display(QString("%1").arg(current_Score, 3, 10, QChar('0')));//设置LCD成绩
	lcdScore->setVisible(true);//显示LCD面板
	restartButton->setVisible(false);//按键设为不可见
	quitButton->setVisible(false);//按键设为不可见
	scoreBoard->setVisible(false);//成绩面板不可见
	gameOverLabel->setVisible(false);
	flappyBird->setVisible(false);
	guide->setVisible(false);
	roadOne->beginMove();//道路开始移动
	roadTwo->beginMove();//道路开始移动

	isBegin = true;//游戏已经开始
	
	gameLoopTimer->start(GAME_LOOP_TIME);//开始游戏循环的计时

	connect(flyBird, SIGNAL(adjustTimer()), gameLoopTimer, SLOT(start()));//小鸟调整计时器的函数和计时器
}

void gameController::gameLoop()
{
	distance--;
	if (!distance)
	{
		QTime time = QTime::currentTime();//获取当前时间
		qsrand(time.msec() + time.second() * 1000);//设置随机种子

		if (3 >= qrand() % 6)//产生新管道的概率为三分之二
		{
			barrier *barrier1 = new barrier();//建立新管道
			scene->addItem(barrier1);//新管道添加到场景中
			barrierList.append(barrier1);//管道列表中添加新管道
			connect(barrier1, SIGNAL(add_Score()), this, SLOT(add_Score()));//关联新管道加分的信号槽
		}

		distance = BARRIER_DISTANCE + qrand() % 7 - 3;//重新设置distance
	}

	flyBird->riseAndfall();//看看是不是需要下降

	if (flyBird->checkCollide())//是否碰撞
	{
		gameOver();
	}
}

bool gameController::eventFilter(QObject *object, QEvent *event)//重写事件过滤器
{	
	if (event->type() == QEvent::KeyPress)//判断是否为键盘按键
	{
		QKeyEvent *key = static_cast<QKeyEvent *>(event);
		if (!isBegin);//如果游戏没开始就不处理事件
		else
			flyBird->keyPressEvent(key);//事件发送给小鸟的按键事件处理
		return true;
	}
// 	else if (event->type() == QEvent::MouseButtonPress)//判断是否为鼠标按键
// 		{
// 			QMouseEvent *key = static_cast<QMouseEvent *>(event);
// 			if (!isBegin);//如果游戏没开始就不处理事件
// 			else
// 				flyBird->mousePressEvent(key);//事件发送给小鸟的按键事件处理
// 			return true;
// 		}
	else
		return QObject::eventFilter(object, event);//转发给默认的事件过滤器
}

void gameController::gameOver()
{
	disconnect(flyBird, SIGNAL(adjustTimer()), gameLoopTimer, SLOT(start()));//取消关联信号槽
	gameLoopTimer->stop();//停止计时
	roadOne->stopMove();//停止移动
	roadTwo->stopMove();//停止移动
	flyBird->stopMove();
	foreach(barrier *bar, barrierList)//管道停止移动
		bar->stopMove();
	isBegin = false;//不在游戏中
	if (current_Score > high_Score) high_Score = current_Score;//设置最高分
	//绘制成绩界面
	pixmap.load(":resource/Resource/scoreboard.png");
	QPixmap pixmap2;
	QPainter painter(&pixmap);
	painter.setFont(QFont("Arial", 15, 20));
	painter.setPen(Qt::white);
	painter.drawText(238, 65, QString("%1").arg(current_Score, 3, 10, QChar('0')));//当前分数
	painter.drawText(238, 122, QString("%1").arg(high_Score, 3, 10, QChar('0')));//高分榜
	if (current_Score < 10)//铜牌
		pixmap2.load(":resource/Resource/medal2.png");
	else if (current_Score < 30)//银牌
		pixmap2.load(":resource/Resource/medal1.png");
	else//金牌
		pixmap2.load(":resource/Resource/medal0.png");
	painter.drawPixmap(30, 50, pixmap2);
	scoreBoard->setPixmap(pixmap);//载入分数榜

	lcdScore->setVisible(false);//设置LCD面板为不可见
	restartButton->setVisible(true);
	quitButton->setVisible(true);
	scoreBoard->setVisible(true);
	gameOverLabel->setVisible(true);
}

void gameController::add_Score()
{
	media->play();//播放加分的音频
	current_Score++;//加分
	lcdScore->display(QString("%1").arg(current_Score, 3, 10, QChar('0')));//设置LCD成绩
}

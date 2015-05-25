#include "gamecontroller.h"

gameController::gameController(QGraphicsScene &Scene, QObject *parent) :
QObject(parent)
{
	scene = &Scene;
	current_Score = 0;//初始分数为0
	distance = BARRIER_DISTANCE;//游戏每循环diastance次,进行一次判断是否产生新的管道,避免管道相互距离过紧

	score = new QLabel("0");
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
	scene->addWidget(score);

	score->setFont(QFont("微软雅黑", 13, 4, false));//设置显示的成绩的字体
	score->setGeometry(50, 50, 25, 25);//设置显示成绩的标签的位置及大小
	score->setStyleSheet("background-color:QColor(197, 240, 255, 130)");//设置样式表
	score->setAlignment(Qt::AlignHCenter);//设置字居中
	media->setMedia(QUrl("qrc:/resource/Music/past.mp3"));//设置通过管道时播放的音频
	media->setVolume(60);
	initReadyWidget();//初始化选择界面

	connect(gameLoopTimer, SIGNAL(timeout()), this, SLOT(gameLoop()));//关联计时器和游戏循环函数
}

void gameController::initReadyWidget()
{

	readyWidget = new QWidget();
	readyWidget->setStyleSheet("QWidget{background-image:url(:/resource/Resource/ready.png)}");//设置选择界面的背景图片
// 	QPainter painter(readyWidget);
// 	pixmapBkgnd.load("./Resource/ready.png");
// 	painter.drawPixmap(0, 0, width(), height(), QPixmap("./Resource/ready.png"));
	readyAnimation = new QPropertyAnimation(readyWidget, "pos");//动画
	quitButton = new QPushButton(readyWidget);//新建按钮
	restartButton = new QPushButton(readyWidget);//新建按钮
	

	lcdScore = new QLCDNumber(3, readyWidget);//成绩面板
	lcdScore->setGeometry(152, 140, 80, 50);//设置位置和大小
	lcdScore->setLineWidth(0);
	lcdScore->setStyleSheet("QLCDNumber{color: white}");
	lcdScore->setVisible(false);//初始设为不可见
	
	restartButton->setStyleSheet("QPushButton{border-image:url(:resource/Resource/replay.png);}"
		"QPushButton:pressed{margin: 2px 2px 2px 2px;}");//设置样式表
	quitButton->setStyleSheet("QPushButton{border-image:url(:resource/Resource/quit.png);}"
		"QPushButton:pressed{margin: 2px 2px 2px 2px;}");//设置样式表
	quitButton->setGeometry(137, 404, 110, 35);//设置按钮的位置和大小
	restartButton->setGeometry(137, 360, 110, 35);//设置按钮的位置和大小

	readyWidget->setFixedSize(WIDTH, HEIGHT);//设置选择界面固定大小
	scene->addWidget(readyWidget)->setZValue(1);//添加选择界面到场景中并设置选择界面在场景中Z轴的坐标

	connect(quitButton, SIGNAL(clicked()), this, SIGNAL(close()));//关联信号槽
	connect(restartButton, SIGNAL(clicked()), this, SLOT(readyWidgetMove()));//关联信号槽

	scene->installEventFilter(this);//给场景添加事件过滤器
}

void gameController::readyWidgetMove()
{
	readyAnimation->setDuration(1300);//选择界面移动到下面 相当于隐藏起来  动画时间为1.3秒
	readyAnimation->setStartValue(readyWidget->pos());
	readyAnimation->setEndValue(QPoint(readyWidget->pos().x(), readyWidget->pos().y() + 450));
	readyAnimation->start();
	connect(readyAnimation, SIGNAL(finished()), this, SLOT(startGame()));//关联信号槽
}

void gameController::startGame()
{
	current_Score = 0;//分数清零
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
		if (!isBegin);//如果游戏没开始就不处理时间
		else
			flyBird->keyPressEvent(key);//时间发送给小鸟的按键事件处理
		return true;
	}
	else
		return QObject::eventFilter(object, event);//转发给默认的事件过滤器
}

void gameController::gameOver()
{
	disconnect(flyBird, SIGNAL(adjustTimer()), gameLoopTimer, SLOT(start()));//取消关联信号槽
	gameLoopTimer->stop();//停止计时
	roadOne->stopMove();//停止移动
	roadTwo->stopMove();//停止移动
	score->setText("0");//分数标签清零
	isBegin = false;//不在游戏中

	foreach(barrier *bar, barrierList)//本次游戏的管道都删除掉
		delete bar;
	
	lcdScore->display(QString("%1").arg(current_Score,3,10,QChar('0')));//设置LCD成绩
	lcdScore->setVisible(true);//设置LCD面板为可见

	barrierList.clear();//清除列表
	scene->update();//更新场景
	readyAnimation->setDuration(1300);//选择界面上移
	readyAnimation->setStartValue(readyWidget->pos());
	readyAnimation->setEndValue(QPoint(readyWidget->pos().x(), readyWidget->pos().y() - 450));
	readyAnimation->start();

	disconnect(readyAnimation, SIGNAL(finished()), this, SLOT(startGame()));//取消信号槽的关联
	connect(readyAnimation, SIGNAL(finished()), flyBird, SLOT(reset()));//动画结束后小鸟重置
}

void gameController::add_Score()
{
	media->play();//播放加分的音频
	current_Score++;//加分
	score->setText(QString::number(current_Score));//设置分数标签显示的分数
}

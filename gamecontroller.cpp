#include "gamecontroller.h"
#include<iostream>
gameController::gameController(QGraphicsScene &Scene, QObject *parent) :
QObject(parent)
{
	scene = &Scene;
	current_Score = 0;//��ʼ����Ϊ0
	distance = BARRIER_DISTANCE;//��Ϸÿѭ��diastance��,����һ���ж��Ƿ�����µĹܵ�,����ܵ��໥�������

	roadOne = new roadItem(1);//��·������,ѭ���ƶ�
	roadTwo = new roadItem(2);
	flyBird = new bird();//��ʼ��С��
	media = new QMediaPlayer();
	gameLoopTimer = new QTimer(this);//��Ϸѭ���ļ�ʱ��

	isBegin = false;//��Ϸ���Ϊδ��ʼ
	//�������м�����Ŀ(��ϷԪ��)
	scene->addItem(roadOne);
	scene->addItem(roadTwo);
	scene->addItem(flyBird);
	
	media->setMedia(QUrl("qrc:/resource/Music/past.mp3"));//����ͨ���ܵ�ʱ���ŵ���Ƶ
	media->setVolume(60);
	initReadyWidget();//��ʼ��ѡ�����

	connect(gameLoopTimer, SIGNAL(timeout()), this, SLOT(gameLoop()));//������ʱ������Ϸѭ������
}

void gameController::initReadyWidget()
{
	scoreBoard = new QLabel();//�ɼ����
	quitButton = new QPushButton();//�½���ť
	restartButton = new QPushButton();//�½���ť
	lcdScore = new QLCDNumber(3);//LCD������ʾ
	flappyBird = new QLabel();//����ͼ
	guide = new QLabel();//��Ϸָ��ͼ
	gameOverLabel = new QLabel();//��Ϸ����ͼ

	high_Score = 0;
	//flappy���ֵ�label
	flappyBird->setPixmap(QPixmap(":resource/Resource/flappybird.png"));
	flappyBird->setStyleSheet("background-color:transparent");
	flappyBird->setGeometry(100, 70, 184, 57);
	//��Ϸָ����label
	guide->setPixmap(QPixmap(":resource/Resource/guide.png"));
	guide->setStyleSheet("background-color:transparent");
	guide->setGeometry(109, 120, 166, 193);
	//��Ϸ������label
	gameOverLabel->setPixmap(QPixmap(":resource/Resource/gameover.png"));
	gameOverLabel->setStyleSheet("background-color:transparent");
	gameOverLabel->setGeometry(64, 70, 256, 57);
	gameOverLabel->setVisible(false);//��ʼ��Ϊ���ɼ�
	//�ɼ����
	scoreBoard->setGeometry(42, 150, 300, 153);
	scoreBoard->setWindowOpacity(0.9);//͸����10%
	scoreBoard->setStyleSheet("background-color:transparent");
	scoreBoard->setVisible(false);
	//LCD�ɼ�
	lcdScore->setGeometry(50, 50, 80, 50);//����λ�úʹ�С
	lcdScore->setLineWidth(0);
	lcdScore->setStyleSheet("QLCDNumber{color: white;background-color:transparent};");
	//��ʽ��Ҫһ�ζ����꣬��Ȼ�������߰���ĺ�Ԥ�ڲ���
	lcdScore->display(QString("%1").arg(0, 3, 10, QChar('0')));//����LCD�ɼ�
	lcdScore->setVisible(false);//��ʼ��Ϊ���ɼ�
	
	restartButton->setStyleSheet("QPushButton{background-color:transparent;"
		"border-image:url(:resource/Resource/replay.png);}"
		"QPushButton:pressed{margin: 2px 2px 2px 2px;}");//������ʽ��
	restartButton->setGeometry(137, 360, 110, 35);//���ð�ť��λ�úʹ�С
	quitButton->setStyleSheet("QPushButton{background-color:transparent;"
		"border-image:url(:resource/Resource/quit.png);}"
		"QPushButton:pressed{margin: 2px 2px 2px 2px;}");//������ʽ��
	quitButton->setGeometry(137, 404, 110, 35);//���ð�ť��λ�úʹ�С

	scene->addWidget(lcdScore)->setZValue(1);//��ӷ�����ʾ���������в�����ѡ������ڳ�����Z�������
	scene->addWidget(restartButton)->setZValue(1);//��ӿ�ʼ��ť�������в�����ѡ������ڳ�����Z�������
	scene->addWidget(quitButton)->setZValue(1);//����˳���ť�������в�����ѡ������ڳ�����Z�������
	scene->addWidget(scoreBoard)->setZValue(1);//��ӳɼ��嵽�����в�����ѡ������ڳ�����Z�������
	scene->addWidget(gameOverLabel)->setZValue(1);//�����Ϸ������嵽�����в�����ѡ������ڳ�����Z�������
	scene->addWidget(flappyBird);//��ӱ�����嵽������
	scene->addWidget(guide);//�����Ϸָ����������
	//scene->addWidget(medal)->setZValue(2);//��ӳɼ��嵽�����в�����ѡ������ڳ�����Z�������

	connect(quitButton, SIGNAL(clicked()), this, SIGNAL(close()));//�����źŲ�
	connect(restartButton, SIGNAL(clicked()), this, SLOT(startGame()));//�����źŲ�

	scene->installEventFilter(this);//����������¼�������
}

void gameController::startGame()
{
	foreach(barrier *bar, barrierList)//��չܵ���Ϣ
		delete bar;
	barrierList.clear();//����б�
	scene->update();//���³���
	flyBird->reset();//����С��
	current_Score = 0;//��������
	lcdScore->display(QString("%1").arg(current_Score, 3, 10, QChar('0')));//����LCD�ɼ�
	lcdScore->setVisible(true);//��ʾLCD���
	restartButton->setVisible(false);//������Ϊ���ɼ�
	quitButton->setVisible(false);//������Ϊ���ɼ�
	scoreBoard->setVisible(false);//�ɼ���岻�ɼ�
	gameOverLabel->setVisible(false);
	flappyBird->setVisible(false);
	guide->setVisible(false);
	roadOne->beginMove();//��·��ʼ�ƶ�
	roadTwo->beginMove();//��·��ʼ�ƶ�

	isBegin = true;//��Ϸ�Ѿ���ʼ
	
	gameLoopTimer->start(GAME_LOOP_TIME);//��ʼ��Ϸѭ���ļ�ʱ

	connect(flyBird, SIGNAL(adjustTimer()), gameLoopTimer, SLOT(start()));//С�������ʱ���ĺ����ͼ�ʱ��
}

void gameController::gameLoop()
{
	distance--;
	if (!distance)
	{
		QTime time = QTime::currentTime();//��ȡ��ǰʱ��
		qsrand(time.msec() + time.second() * 1000);//�����������

		if (3 >= qrand() % 6)//�����¹ܵ��ĸ���Ϊ����֮��
		{
			barrier *barrier1 = new barrier();//�����¹ܵ�
			scene->addItem(barrier1);//�¹ܵ���ӵ�������
			barrierList.append(barrier1);//�ܵ��б�������¹ܵ�
			connect(barrier1, SIGNAL(add_Score()), this, SLOT(add_Score()));//�����¹ܵ��ӷֵ��źŲ�
		}

		distance = BARRIER_DISTANCE + qrand() % 7 - 3;//��������distance
	}

	flyBird->riseAndfall();//�����ǲ�����Ҫ�½�

	if (flyBird->checkCollide())//�Ƿ���ײ
	{
		gameOver();
	}
}

bool gameController::eventFilter(QObject *object, QEvent *event)//��д�¼�������
{	
	if (event->type() == QEvent::KeyPress)//�ж��Ƿ�Ϊ���̰���
	{
		QKeyEvent *key = static_cast<QKeyEvent *>(event);
		if (!isBegin);//�����Ϸû��ʼ�Ͳ������¼�
		else
			flyBird->keyPressEvent(key);//�¼����͸�С��İ����¼�����
		return true;
	}
// 	else if (event->type() == QEvent::MouseButtonPress)//�ж��Ƿ�Ϊ��갴��
// 		{
// 			QMouseEvent *key = static_cast<QMouseEvent *>(event);
// 			if (!isBegin);//�����Ϸû��ʼ�Ͳ������¼�
// 			else
// 				flyBird->mousePressEvent(key);//�¼����͸�С��İ����¼�����
// 			return true;
// 		}
	else
		return QObject::eventFilter(object, event);//ת����Ĭ�ϵ��¼�������
}

void gameController::gameOver()
{
	disconnect(flyBird, SIGNAL(adjustTimer()), gameLoopTimer, SLOT(start()));//ȡ�������źŲ�
	gameLoopTimer->stop();//ֹͣ��ʱ
	roadOne->stopMove();//ֹͣ�ƶ�
	roadTwo->stopMove();//ֹͣ�ƶ�
	flyBird->stopMove();
	foreach(barrier *bar, barrierList)//�ܵ�ֹͣ�ƶ�
		bar->stopMove();
	isBegin = false;//������Ϸ��
	if (current_Score > high_Score) high_Score = current_Score;//������߷�
	//���Ƴɼ�����
	pixmap.load(":resource/Resource/scoreboard.png");
	QPixmap pixmap2;
	QPainter painter(&pixmap);
	painter.setFont(QFont("Arial", 15, 20));
	painter.setPen(Qt::white);
	painter.drawText(238, 65, QString("%1").arg(current_Score, 3, 10, QChar('0')));//��ǰ����
	painter.drawText(238, 122, QString("%1").arg(high_Score, 3, 10, QChar('0')));//�߷ְ�
	if (current_Score < 10)//ͭ��
		pixmap2.load(":resource/Resource/medal2.png");
	else if (current_Score < 30)//����
		pixmap2.load(":resource/Resource/medal1.png");
	else//����
		pixmap2.load(":resource/Resource/medal0.png");
	painter.drawPixmap(30, 50, pixmap2);
	scoreBoard->setPixmap(pixmap);//���������

	lcdScore->setVisible(false);//����LCD���Ϊ���ɼ�
	restartButton->setVisible(true);
	quitButton->setVisible(true);
	scoreBoard->setVisible(true);
	gameOverLabel->setVisible(true);
}

void gameController::add_Score()
{
	media->play();//���żӷֵ���Ƶ
	current_Score++;//�ӷ�
	lcdScore->display(QString("%1").arg(current_Score, 3, 10, QChar('0')));//����LCD�ɼ�
}

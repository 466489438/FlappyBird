#include "gamecontroller.h"

gameController::gameController(QGraphicsScene &Scene, QObject *parent) :
QObject(parent)
{
	scene = &Scene;
	current_Score = 0;//��ʼ����Ϊ0
	distance = BARRIER_DISTANCE;//��Ϸÿѭ��diastance��,����һ���ж��Ƿ�����µĹܵ�,����ܵ��໥�������

	score = new QLabel("0");
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
	scene->addWidget(score);

	score->setFont(QFont("΢���ź�", 13, 4, false));//������ʾ�ĳɼ�������
	score->setGeometry(50, 50, 25, 25);//������ʾ�ɼ��ı�ǩ��λ�ü���С
	score->setStyleSheet("background-color:QColor(197, 240, 255, 130)");//������ʽ��
	score->setAlignment(Qt::AlignHCenter);//�����־���
	media->setMedia(QUrl("qrc:/resource/Music/past.mp3"));//����ͨ���ܵ�ʱ���ŵ���Ƶ
	media->setVolume(60);
	initReadyWidget();//��ʼ��ѡ�����

	connect(gameLoopTimer, SIGNAL(timeout()), this, SLOT(gameLoop()));//������ʱ������Ϸѭ������
}

void gameController::initReadyWidget()
{

	readyWidget = new QWidget();
	readyWidget->setStyleSheet("QWidget{background-image:url(:/resource/Resource/ready.png)}");//����ѡ�����ı���ͼƬ
// 	QPainter painter(readyWidget);
// 	pixmapBkgnd.load("./Resource/ready.png");
// 	painter.drawPixmap(0, 0, width(), height(), QPixmap("./Resource/ready.png"));
	readyAnimation = new QPropertyAnimation(readyWidget, "pos");//����
	quitButton = new QPushButton(readyWidget);//�½���ť
	restartButton = new QPushButton(readyWidget);//�½���ť
	

	lcdScore = new QLCDNumber(3, readyWidget);//�ɼ����
	lcdScore->setGeometry(152, 140, 80, 50);//����λ�úʹ�С
	lcdScore->setLineWidth(0);
	lcdScore->setStyleSheet("QLCDNumber{color: white}");
	lcdScore->setVisible(false);//��ʼ��Ϊ���ɼ�
	
	restartButton->setStyleSheet("QPushButton{border-image:url(:resource/Resource/replay.png);}"
		"QPushButton:pressed{margin: 2px 2px 2px 2px;}");//������ʽ��
	quitButton->setStyleSheet("QPushButton{border-image:url(:resource/Resource/quit.png);}"
		"QPushButton:pressed{margin: 2px 2px 2px 2px;}");//������ʽ��
	quitButton->setGeometry(137, 404, 110, 35);//���ð�ť��λ�úʹ�С
	restartButton->setGeometry(137, 360, 110, 35);//���ð�ť��λ�úʹ�С

	readyWidget->setFixedSize(WIDTH, HEIGHT);//����ѡ�����̶���С
	scene->addWidget(readyWidget)->setZValue(1);//���ѡ����浽�����в�����ѡ������ڳ�����Z�������

	connect(quitButton, SIGNAL(clicked()), this, SIGNAL(close()));//�����źŲ�
	connect(restartButton, SIGNAL(clicked()), this, SLOT(readyWidgetMove()));//�����źŲ�

	scene->installEventFilter(this);//����������¼�������
}

void gameController::readyWidgetMove()
{
	readyAnimation->setDuration(1300);//ѡ������ƶ������� �൱����������  ����ʱ��Ϊ1.3��
	readyAnimation->setStartValue(readyWidget->pos());
	readyAnimation->setEndValue(QPoint(readyWidget->pos().x(), readyWidget->pos().y() + 450));
	readyAnimation->start();
	connect(readyAnimation, SIGNAL(finished()), this, SLOT(startGame()));//�����źŲ�
}

void gameController::startGame()
{
	current_Score = 0;//��������
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
		if (!isBegin);//�����Ϸû��ʼ�Ͳ�����ʱ��
		else
			flyBird->keyPressEvent(key);//ʱ�䷢�͸�С��İ����¼�����
		return true;
	}
	else
		return QObject::eventFilter(object, event);//ת����Ĭ�ϵ��¼�������
}

void gameController::gameOver()
{
	disconnect(flyBird, SIGNAL(adjustTimer()), gameLoopTimer, SLOT(start()));//ȡ�������źŲ�
	gameLoopTimer->stop();//ֹͣ��ʱ
	roadOne->stopMove();//ֹͣ�ƶ�
	roadTwo->stopMove();//ֹͣ�ƶ�
	score->setText("0");//������ǩ����
	isBegin = false;//������Ϸ��

	foreach(barrier *bar, barrierList)//������Ϸ�Ĺܵ���ɾ����
		delete bar;
	
	lcdScore->display(QString("%1").arg(current_Score,3,10,QChar('0')));//����LCD�ɼ�
	lcdScore->setVisible(true);//����LCD���Ϊ�ɼ�

	barrierList.clear();//����б�
	scene->update();//���³���
	readyAnimation->setDuration(1300);//ѡ���������
	readyAnimation->setStartValue(readyWidget->pos());
	readyAnimation->setEndValue(QPoint(readyWidget->pos().x(), readyWidget->pos().y() - 450));
	readyAnimation->start();

	disconnect(readyAnimation, SIGNAL(finished()), this, SLOT(startGame()));//ȡ���źŲ۵Ĺ���
	connect(readyAnimation, SIGNAL(finished()), flyBird, SLOT(reset()));//����������С������
}

void gameController::add_Score()
{
	media->play();//���żӷֵ���Ƶ
	current_Score++;//�ӷ�
	score->setText(QString::number(current_Score));//���÷�����ǩ��ʾ�ķ���
}

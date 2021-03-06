#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    scene = new QGraphicsScene(this);//建立一个场景
    scene->setSceneRect(0, 0, WIDTH, HEIGHT);//设置场景的位置和大小
    view = new QGraphicsView(scene, this);//建立一个视图,并添加场景到该视图中
    controller = new gameController(*scene, this);//建立一个游戏控制器

    view->setBackgroundBrush(QPixmap(":/resource/Resource/background.png"));//设置视图的背景图片
    view->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers), this));//使用OpenGL绘制
    view->setFixedSize(WIDTH + 2, HEIGHT + 2);//设置视图固定大小

    setWindowFlags(Qt::FramelessWindowHint);//设置主窗口为无边框
    setFixedSize(WIDTH + 2, HEIGHT + 2);//设置主窗口固定大小
    setCentralWidget(view);//把视图添加到主窗口的中心区域

    connect(controller, SIGNAL(close()), this, SLOT(close()));//关联controller的close信号和主窗口的close槽
}

MainWindow::~MainWindow()
{
}

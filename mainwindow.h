#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QMainWindow>
#include <QtMultimedia/QMediaPlayer>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsObject>
#include <QPropertyAnimation>
#include <QGraphicsProxyWidget>
#include <QLabel>
#include <QTransform>
#include <QPainterPath>
#include <QPushButton>
#include <QVBoxLayout>
#include <QKeyEvent>
#include <QTime>
#include <QMatrix>
#include <QImage>
#include <QTimer>
#include <QPainter>
#include <QDebug>
#include <QUrl>
#include <QtOpenGL/QGLWidget>
#include <QtOpenGL/QGLFormat>
#include "barrier.h"
#include "define.h"
#include "roaditem.h"
#include "bird.h"
#include "gamecontroller.h"

class gameController;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    QGraphicsView *view;
    QGraphicsScene *scene;
    gameController *controller;
};

#endif // MAINWINDOW_H

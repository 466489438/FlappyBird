#ifndef SCOREBOARD_H
#define SCOREBOARD_H

#include <QObject>

#include <QPixmap>
#include <QImage>
#include <QGraphicsView>
class scoreBoard : public QObject
{
	Q_OBJECT

public:
	scoreBoard(QObject *parent);
	~scoreBoard();

private:
	
	private slots :
		void paintEvent(QPaintEvent *);
};

#endif // SCOREBOARD_H

#include "scoreboard.h"

scoreBoard::scoreBoard(QObject *parent)
	: QObject(parent)
{

}

void scoreBoard::paintEvent(QPaintEvent *)
{
	QPainter painte(this);
	QRectF pt(70.0, 50.0, 320.0, 240.0);
	QRectF source(x_s - 3 * x0, y_s - 3 * y0, w_s - x0*.5, h_s - y0*0.5);
	painte.drawImage(pt, img, source);
	update();
}
scoreBoard::~scoreBoard()
{

}

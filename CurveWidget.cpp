#include "CurveWidget.h"
#include <QDesktopWidget>
#include <QRect>
#include <QApplication>
#include <QMouseEvent>
#include <QWheelEvent>

CurveWidget::CurveWidget(QWidget *parent) : QWidget(parent)
{
	QPalette Pal(palette());
	Pal.setColor(QPalette::Background, QColor(38, 38, 38));
	setAutoFillBackground(true);
	setPalette(Pal);

	QDesktopWidget *desktop = QApplication::desktop();
	QRect screenSize = desktop->availableGeometry(this);
	resize(QSize(screenSize.width() * 0.7f, screenSize.height() * 0.7f));

	NormalizeView();
}

void drawCorner(const QPoint& center, QPainter* painter)
{
	static const int size1 = 3;
	painter->setBrush(QBrush("#ffffff"));
	painter->drawRect(center.x() - size1, center.y() - size1, size1 * 2, size1 * 2);
}

void CurveWidget::paintEvent(QPaintEvent *e)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::HighQualityAntialiasing, false);

	//painter.setRenderHint(QPainter::Antialiasing, false);
	int gridWidth = 1;

	//
	// Thin grid lines
	//
	//QColor gridColorThin(46, 46, 46);
	//painter.setPen(QPen(gridColorThin, gridWidth, Qt::SolidLine, Qt::FlatCap));
	//for (int i = 1; i <= size().width() * 10 / GetAnalyticUnitInPixels(); i++)
	//{
	//	painter.drawLine(i * GetAnalyticUnitInPixels() / 10, 0, i * GetAnalyticUnitInPixels() / 10, size().height());
	//}
	//painter.setPen(QPen(gridColorThin, gridWidth, Qt::SolidLine, Qt::FlatCap));
	//for (int i = 1; i <= size().height() * 10 / GetAnalyticUnitInPixels(); i++)
	//{
	//	painter.drawLine(0, i * GetAnalyticUnitInPixels() / 10, size().width(), i * GetAnalyticUnitInPixels() / 10);
	//}


	//
	// Fat grid lines
	//
	//QColor gridColorFat(80, 80, 80);
	//painter.setPen(QPen(gridColorFat, gridWidth, Qt::SolidLine, Qt::FlatCap));
	//for (int i = 1; i <= size().width() / GetAnalyticUnitInPixels(); i++)
	//{
	//	painter.drawLine(i * GetAnalyticUnitInPixels(), 0, i * GetAnalyticUnitInPixels(), size().height());
	//}
	//painter.setPen(QPen(gridColorFat, gridWidth, Qt::SolidLine, Qt::FlatCap));
	//for (int i = 1; i <= size().height() / GetAnalyticUnitInPixels(); i++)
	//{
	//	painter.drawLine(0, i * GetAnalyticUnitInPixels(), size().width(), i * GetAnalyticUnitInPixels());
	//}


	QColor gridColorThin(46, 46, 46);
	painter.setPen(QPen(gridColorThin, gridWidth, Qt::SolidLine, Qt::FlatCap));

	// horizontal
	float y = ToAnalyticCoordinates(QPoint(0, 0)).y();
	int y_min = ceil(y);
	y = ToAnalyticCoordinates(QPoint(0, size().height())).y();
	int y_max = floor(y);
	for (int i = y_max; i <= y_min; i++)
	{
		int pp = ToCanvasCoordinates(QPointF(0, i)).y();
		painter.drawLine(0, pp, size().width(), pp);
	}

	// vertical
	float x = ToAnalyticCoordinates(QPoint(0, 0)).x();
	int x_min = ceil(x);
	x = ToAnalyticCoordinates(QPoint(size().width(), 0)).x();
	int x_max = floor(x);
	for (int i = x_min; i <= x_max; i++)
	{
		int pp = ToCanvasCoordinates(QPointF(i, 0)).x();
		painter.drawLine(pp, 0, pp, size().height());
	}

	//
	// Axis
	//
	QColor gridColorFat(80, 80, 80);
	painter.setPen(QPen(gridColorFat, gridWidth, Qt::SolidLine, Qt::FlatCap));
	QPoint p = ToCanvasCoordinates(QPointF(0,0));
	if (0 <= p.x() && p.x() <= size().width())
		painter.drawLine(p.x(), 0, p.x(), size().height());
	if (0 <= p.y() && p.y() <= size().height())
		painter.drawLine(0, p.y(), size().width(), p.y());

	/* Make the Gradient for this line. */
	//const QPointF start{ 0,0 };
	//const QPointF end{ 100,100 };
	//QLinearGradient gradient(start, end);
	//QColor color(123, 123, 123); //some color
	//color.setAlphaF(0.1); //change alpha
	//gradient.setColorAt(0, color);
	//color.setAlphaF(0.1); //change alpha again
	//gradient.setColorAt(1, color );

	painter.setRenderHint(QPainter::HighQualityAntialiasing, true);

	//
	// Curves
	//
	painter.setPen(QPen(Qt::green, 2, Qt::SolidLine, Qt::RoundCap));
	painter.drawLine(ToCanvasCoordinates(QPoint(0,0)), ToCanvasCoordinates(QPoint(1, 1)));


	QPainterPath path;
	path.moveTo(20, 80);
	path.lineTo(20, 30);
	path.cubicTo(80, 0, 50, 50, 80, 80);
	painter.drawPath(path);

	painter.setRenderHint(QPainter::HighQualityAntialiasing, false);

	//
	// Rects
	//
	QColor cornerColor(0, 0, 0);
	painter.setPen(QPen(cornerColor, 1, Qt::SolidLine, Qt::FlatCap));
	drawCorner(ToCanvasCoordinates(QPointF(1, 1)), &painter);
	drawCorner(ToCanvasCoordinates(QPointF(0, 0)), &painter);
}

void CurveWidget::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::MouseButton::LeftButton)
	{
		dragging = 1;
		startMousePos = event->pos();
		startPixelsOffset = pixelsOffset;
	}
}

void CurveWidget::mouseReleaseEvent(QMouseEvent *event)
{
	dragging = 0;
}

void CurveWidget::mouseMoveEvent(QMouseEvent *event)
{
	if (dragging)
	{
		pixelsOffset = startPixelsOffset + (event->pos() - startMousePos);
		repaint();
	}
}

void CurveWidget::wheelEvent(QWheelEvent *event)
{
	int numDegrees = event->delta();
	scale *= float((numDegrees > 0) ? 1.05f : 0.95f);
	event->accept();
	repaint();
}

void CurveWidget::resizeEvent(QResizeEvent *event)
{
	float tempScaleX = (float)event->size().width() / event->oldSize().width();
	float tempScaleY = (float)event->size().height() / event->oldSize().height();
	if (tempScaleX < 0.0001f || tempScaleY < 0.0001f)
	{
		QWidget::resizeEvent(event);
		return;
	}
	pixelsOffset = QPointF((float)pixelsOffset.x() * tempScaleX, pixelsOffset.y() * tempScaleY);
	QWidget::resizeEvent(event);
}

void CurveWidget::NormalizeView()
{
	pixelsOffset = QPointF(size().width() / 2, size().height() / 2);
	scale = 100.0f;
}

QPoint CurveWidget::ToCanvasCoordinates(const QPointF &pos)
{
	return QPoint(pos.x() * scale + pixelsOffset.x(), -pos.y() * scale + pixelsOffset.y());
}

QPointF CurveWidget::ToAnalyticCoordinates(const QPoint &pos)
{
	return QPointF((pos.x() - pixelsOffset.x()) / scale, (-pos.y() + pixelsOffset.y()) / scale);
}

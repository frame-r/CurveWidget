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

void CurveWidget::paintEvent(QPaintEvent *e)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);

	painter.setRenderHint(QPainter::Antialiasing, false);
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

	QColor gridColorFat(80, 80, 80);
	painter.setPen(QPen(gridColorFat, gridWidth, Qt::SolidLine, Qt::FlatCap));
	//int unitHorizontalLines = (int)(GetAnalyticUnitInPixels() / size().height());
	//for (int i = 1; i <= unitHorizontalLines; i++)
	//{
	//}
	//float y = ToAnalyticCoordinates(QPoint(0, 0)).y();
	//int y_int = ceil(y);
	//for (int i = 1; i <= size().height() / GetAnalyticUnitInPixels(); i++)

	// Axis
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


	//
	// Curves
	//
	painter.setPen(QPen(Qt::green, 2, Qt::SolidLine, Qt::RoundCap));
	painter.drawLine(ToCanvasCoordinates(QPoint(0,0)), ToCanvasCoordinates(QPoint(1, 1)));

}

void CurveWidget::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::MouseButton::MiddleButton)
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
	return QPointF((pos.x() - pixelsOffset.x()) / scale, (-pos.y() + pixelsOffset.y()) / scale );
}

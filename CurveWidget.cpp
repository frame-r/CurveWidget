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
	QColor gridColorThin(46, 46, 46);
	painter.setPen(QPen(gridColorThin, gridWidth, Qt::SolidLine, Qt::FlatCap));
	for (int i = 1; i <= size().width() * 10 / GetLinesUnit(); i++)
	{
		painter.drawLine(i * GetLinesUnit() / 10, 0, i * GetLinesUnit() / 10, size().height());
	}
	painter.setPen(QPen(gridColorThin, gridWidth, Qt::SolidLine, Qt::FlatCap));
	for (int i = 1; i <= size().height() * 10 / GetLinesUnit(); i++)
	{
		painter.drawLine(0, i * GetLinesUnit() / 10, size().width(), i * GetLinesUnit() / 10);
	}


	//
	// Fat grid lines
	//
	QColor gridColorFat(90, 90, 90);
	painter.setPen(QPen(gridColorFat, gridWidth, Qt::SolidLine, Qt::FlatCap));
	for (int i = 1; i <= size().width() / GetLinesUnit(); i++)
	{
		painter.drawLine(i * GetLinesUnit(), 0, i * GetLinesUnit(), size().height());
	}
	painter.setPen(QPen(gridColorFat, gridWidth, Qt::SolidLine, Qt::FlatCap));
	for (int i = 1; i <= size().height() / GetLinesUnit(); i++)
	{
		painter.drawLine(0, i * GetLinesUnit(), size().width(), i * GetLinesUnit());
	}


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
	painter.drawLine(ToCanvasCoordinates(QPoint(0,0)), ToCanvasCoordinates(QPoint(GetLinesUnit(), GetLinesUnit())));

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
	int numDegrees = event->delta() / 8;
	int numSteps = numDegrees / 15;
	scale += float(numSteps);
	event->accept();
	repaint();
}

float CurveWidget::GetLinesUnit()
{
	return scale;
}

void CurveWidget::NormalizeView()
{
	pixelsOffset = QPoint(size().width() / 2, size().height() / 2);
	scale = 100.0f;
}

QPoint CurveWidget::ToCanvasCoordinates(const QPoint &pos)
{
	return QPoint(pixelsOffset.x() + pos.x(), pixelsOffset.y() - pos.y());
}

QPoint CurveWidget::ToAnalyticCoordinates(const QPoint &pos)
{
	return QPoint(pos.x() - pixelsOffset.x(), pixelsOffset.y() - pos.y());
}

#include "CurveWidget.h"
#include <QDesktopWidget>
#include <QRect>
#include <QApplication>

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

	/* Make the Gradient for this line. */
	//const QPointF start{ 0,0 };
	//const QPointF end{ 100,100 };
	//QLinearGradient gradient(start, end);
	//QColor color(123, 123, 123); //some color
	//color.setAlphaF(0.9); //change alpha
	//gradient.setColorAt(0, color);
	//color.setAlphaF(0.1); //change alpha again
	//gradient.setColorAt(1, color );
	painter.setRenderHint(QPainter::Antialiasing, false);
	QColor gridColor(90, 90, 90);
	int gridWidth = 1;

	for (int i = 1; i <= size().width() / getLinesUnit(); i++)
	{
		painter.setPen(QPen(gridColor, gridWidth, Qt::SolidLine, Qt::FlatCap));
		painter.drawLine(i * getLinesUnit(), 0, i * getLinesUnit(), size().height());
	}

	for (int i = 1; i <= size().height() / getLinesUnit(); i++)
	{
		painter.setPen(QPen(gridColor, gridWidth, Qt::SolidLine, Qt::FlatCap));
		painter.drawLine(0, i * getLinesUnit(), size().width(), i * getLinesUnit());
	}

	painter.setPen(QPen(Qt::green, 2, Qt::SolidLine, Qt::RoundCap));
	painter.drawLine(ToCanvasCoordinates(QPoint(0,0)), ToCanvasCoordinates(QPoint(getLinesUnit(), getLinesUnit())));

}

int CurveWidget::getLinesUnit()
{
	return scale;
}

void CurveWidget::NormalizeView()
{
	pixelsOffset = QPoint(size().width() / 2, size().height() / 2);
	scale = 100;
}

QPoint CurveWidget::ToCanvasCoordinates(const QPoint &pos)
{
	return QPoint(pixelsOffset.x() + pos.x(), pixelsOffset.y() - pos.y());
}

QPoint CurveWidget::ToAnalyticCoordinates(const QPoint &pos)
{
	return QPoint(pos.x() - pixelsOffset.x(), pixelsOffset.y() - pos.y());
}

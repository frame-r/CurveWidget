#include "CurveWidget.h"
#include <QDesktopWidget>
#include <QRect>
#include <QApplication>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QMenu>
#include <QAction>

const int MinimumSelectDotsDistance = 6;
const QColor cornerColor(0, 0, 0);
const QColor selectionCornerColor(255, 0, 0);

inline float clamp(float n, float lower, float upper)
{
	return std::max(lower, std::min(n, upper));
}

float QPointLen(const QPoint& q)
{
	return sqrt((float)q.x() * q.x() + q.y() * q.y());
}

float QPointDot(const QPoint& q0, const QPoint& q1)
{
	return (float)q0.x() * q1.x() + q0.y() * q1.y();
}

float PointToSegmentDistance(const QPoint& a, const QPoint& b, const QPoint& p)
{
	QPoint n = b - a;
	QPoint pa = a - p;
	QPoint c = n * (QPointDot( pa, n ) / QPointDot( n, n ));
	QPoint d = pa - c;
	return sqrt( QPointDot( d, d ) );
}


CurveWidget::CurveWidget(QWidget *parent) : QWidget(parent)
{
	setMouseTracking(true);

	QPalette Pal(palette());
	Pal.setColor(QPalette::Background, QColor(38, 38, 38));
	setAutoFillBackground(true);
	setPalette(Pal);

	QDesktopWidget *desktop = QApplication::desktop();
	QRect screenSize = desktop->availableGeometry(this);
	resize(QSize(screenSize.width() * 0.7f, screenSize.height() * 0.7f));

	this->setContextMenuPolicy(Qt::CustomContextMenu);

	connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(ShowContextMenu(const QPoint &)));

	_timer = std::make_unique<QTimer>(this);
	connect(_timer.get(), SIGNAL(timeout()), this, SLOT(_OnTimer()));
	_timer->start(16);

	NormalizeView();
}

void CurveWidget::ShowContextMenu(const QPoint &pos)
{
   QMenu contextMenu(tr("Context menu"), this);

   QAction action1("Add Point", this);
   connect(&action1, SIGNAL(triggered()), this, SLOT(removeDataPoint()));
   contextMenu.addAction(&action1);

   QAction action2("Remove Point", this);
   connect(&action2, SIGNAL(triggered()), this, SLOT(removeDataPoint()));
   contextMenu.addAction(&action2);

   contextMenu.exec(mapToGlobal(pos));
}

void CurveWidget::_OnTimer()
{
	int _mouseUnderDot = -1;
	for (int i = 0; i < _currentCurve.PointsNum(); i++)
	{
		auto dist = (mousePosition - ToCanvasCoordinates(_currentCurve.FetchPoint(i).pos)).manhattanLength();
//		if (i == 0)
//			qDebug() << dist;
		if (dist < MinimumSelectDotsDistance)
		{
			_mouseUnderDot = i;
			break;
		}
	}

	int _mouseUnderSeg = -1;
	for (int i = 0; i < _currentCurve.SugmentsNum(); i++)
	{
		CurveSegment seg = _currentCurve.FetchSegment(i);
		float dist = PointToSegmentDistance(ToCanvasCoordinates(seg.q0), ToCanvasCoordinates(seg.q1), mousePosition);
				if (i == 0)
					qDebug() << dist;
		if (dist < MinimumSelectDotsDistance)
		{
			_mouseUnderSeg = i;
			break;
		}
	}

	if (_mouseUnderDot != -1 && _mouseUnderSeg != -1)
	{
		_mouseUnderSeg = -1;
	}

	if (_mouseUnderDot != mouseUnderDot || _mouseUnderSeg != mouseUnderSeg)
	{
		mouseUnderDot = _mouseUnderDot;
		mouseUnderSeg = _mouseUnderSeg;
		repaint();
	}
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

	//
	// Lines
	//
	//painter.setPen(QPen(Qt::green, 2, Qt::SolidLine, Qt::RoundCap));
	int s = 1;
	painter.setPen(QPen(Qt::red, 1, Qt::SolidLine, Qt::FlatCap));

	for (int i = 0; i < _currentCurve.SugmentsNum(); i++)
	{
		int nextSize = 1;
		if (mouseUnderSeg == i)
		{
			nextSize = 2;
		}

		if (nextSize != s)
		{
			s = nextSize;
			painter.setPen(QPen(Qt::red, nextSize, Qt::SolidLine, Qt::FlatCap));
		}

		CurveSegment seg = _currentCurve.FetchSegment(i);
		painter.drawLine(ToCanvasCoordinates(seg.q0), ToCanvasCoordinates(seg.q1));
	}

	//
	// Rectangle dots
	//
	QColor col = cornerColor;
	painter.setPen(QPen(col, 1, Qt::SolidLine, Qt::FlatCap));

	for (int i = 0; i < _currentCurve.PointsNum(); i++)
	{
		QColor nextCol = cornerColor;
		if (mouseUnderDot == i)
		{
			nextCol = selectionCornerColor;
		}

		if (col != nextCol)
		{
			col = nextCol;
			painter.setPen(QPen(col, 1, Qt::SolidLine, Qt::FlatCap));
		}

		CurvePoint p = _currentCurve.FetchPoint(i);
		drawCorner(ToCanvasCoordinates(p.pos), &painter);
	}

}

void CurveWidget::mouseReleaseEvent(QMouseEvent *event)
{
	isMouseDragging = 0;
}

void CurveWidget::mouseMoveEvent(QMouseEvent *event)
{
	mousePosition = event->pos();

	if (isMouseDragging)
	{
		centerOffset = dragStartPixelsOffset + (event->pos() - dragStartMousePos);
		repaint();
	}
}

void CurveWidget::wheelEvent(QWheelEvent *event)
{
	int numDegrees = event->delta();
	pixelsInUnitScale *= float((numDegrees > 0) ? 1.05f : 0.95f);
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
	centerOffset = QPointF((float)centerOffset.x() * tempScaleX, centerOffset.y() * tempScaleY);
	QWidget::resizeEvent(event);
}

void CurveWidget::NormalizeView()
{
	centerOffset = QPointF(size().width() / 2, size().height() / 2);
	pixelsInUnitScale = 100.0f;
}


void CurveWidget::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::MouseButton::MiddleButton)
	{
		isMouseDragging = 1;
		dragStartMousePos = event->pos();
		dragStartPixelsOffset = centerOffset;
	}
}

QPoint CurveWidget::ToCanvasCoordinates(const QPointF &pos)
{
	return QPoint(pos.x() * pixelsInUnitScale + centerOffset.x(), -pos.y() * pixelsInUnitScale + centerOffset.y());
}

QPointF CurveWidget::ToAnalyticCoordinates(const QPoint &pos)
{
	return QPointF((pos.x() - centerOffset.x()) / pixelsInUnitScale, (-pos.y() + centerOffset.y()) / pixelsInUnitScale);
}

#include "CurveWidget.h"
#include <QDesktopWidget>
#include <QRect>
#include <QApplication>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QMenu>
#include <QAction>

const int MinimumSelectDotsDistance = 9;
const QColor DotEdgeColor(0, 0, 0);
const QColor DotColor(255, 255, 255);
const QColor DotEdgeSelectionColor(255, 0, 0);
const QColor DotSelectionColor(255, 255, 255);
const QColor GridThinColor(46, 46, 46);
const int GridWidth = 1;
const int DotSize = 3;

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
	resize(QSize(1000, 1000));

	this->setContextMenuPolicy(Qt::CustomContextMenu);

	connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(showContextMenu(const QPoint &)));

	_timer = std::make_unique<QTimer>(this);
	connect(_timer.get(), SIGNAL(timeout()), this, SLOT(onTimer()));
	_timer->start(16);

	NormalizeView();
}

void CurveWidget::showContextMenu(const QPoint &pos)
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

void CurveWidget::onTimer()
{
	int isMouseUnderDot[4] = {};

	auto check_dot_intersection = [&](const QPointF& P) -> int
	{
		auto dist = (mousePosition - ToCanvasCoordinates(P)).manhattanLength();
		return (dist < MinimumSelectDotsDistance);
	};

	for (int i = 0; i<4; i++)
		isMouseUnderDot[i] = check_dot_intersection(_currentCurve.dots[i]);

	if (memcmp(isMouseUnderDot, mouseUnderPoint, sizeof(int) * 4))
	{
		memcpy(mouseUnderPoint, isMouseUnderDot, sizeof(int) * 4);
		repaint();
	}
}

void drawDot(const QPoint& center, QPainter* painter)
{
	painter->drawRect(center.x() - DotSize, center.y() - DotSize, DotSize * 2, DotSize * 2);
}

void CurveWidget::paintEvent(QPaintEvent *e)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, false);
	painter.setPen(QPen(GridThinColor, GridWidth, Qt::SolidLine, Qt::FlatCap));

	// Horizontal lines
	//
	float y = ToAnalyticCoordinates(QPoint(0, 0)).y();
	int y_min = ceil(y);
	y = ToAnalyticCoordinates(QPoint(0, size().height())).y();
	int y_max = floor(y);
	for (int i = y_max; i <= y_min; i++)
	{
		int pp = ToCanvasCoordinates(QPointF(0, i)).y();
		painter.drawLine(0, pp, size().width(), pp);
	}

	// Vertical lines
	//
	float x = ToAnalyticCoordinates(QPoint(0, 0)).x();
	int x_min = ceil(x);
	x = ToAnalyticCoordinates(QPoint(size().width(), 0)).x();
	int x_max = floor(x);
	for (int i = x_min; i <= x_max; i++)
	{
		int pp = ToCanvasCoordinates(QPointF(i, 0)).x();
		painter.drawLine(pp, 0, pp, size().height());
	}

	// Axis
	//
	QColor gridColorFat(80, 80, 80);
	painter.setPen(QPen(gridColorFat, GridWidth, Qt::SolidLine, Qt::FlatCap));
	QPoint p = ToCanvasCoordinates(QPointF(0,0));
	if (0 <= p.x() && p.x() <= size().width())
		painter.drawLine(p.x(), 0, p.x(), size().height());
	if (0 <= p.y() && p.y() <= size().height())
		painter.drawLine(0, p.y(), size().width(), p.y());


	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setPen(QPen(Qt::red, 2, Qt::SolidLine, Qt::FlatCap));


	// Curve segments
	//
	constexpr float steps = 40.0f;
	constexpr float step = 1.0f / steps;
	float t = step;

	for (; t <= 1.0f; t+=step)
	{
		QPointF p0 = _currentCurve.Evaluate(t - step);
		QPointF p1 = _currentCurve.Evaluate(t);
		painter.drawLine(ToCanvasCoordinates(p0), ToCanvasCoordinates(p1));
	}
	{
		QPointF p0 = _currentCurve.Evaluate(1.0f - step);
		QPointF p1 = _currentCurve.Evaluate(1.0f);
		painter.drawLine(ToCanvasCoordinates(p0), ToCanvasCoordinates(p1));
	}

	painter.setPen(QPen(Qt::green, 1, Qt::SolidLine, Qt::RoundCap));
	painter.drawLine(ToCanvasCoordinates(_currentCurve.A), ToCanvasCoordinates(_currentCurve.P1));
	painter.drawLine(ToCanvasCoordinates(_currentCurve.B), ToCanvasCoordinates(_currentCurve.P2));
	painter.setRenderHint(QPainter::Antialiasing, false);


	// Dots
	//
	QColor col = DotColor;
	QColor colEdge = DotEdgeColor;
	painter.setPen(QPen(colEdge, 1, Qt::SolidLine, Qt::FlatCap));
	painter.setBrush(QBrush(col));

	for (int i = 0; i < 4; i++)
	{
		QColor nextCol = DotColor;
		QColor nextEdgeCol = DotEdgeColor;

		if (mouseUnderPoint[i])
		{
			nextCol = DotSelectionColor;
			nextEdgeCol = DotEdgeSelectionColor;
		}

		if (col != nextCol || colEdge != nextEdgeCol)
		{
			col = nextCol;
			colEdge = nextEdgeCol;
			painter.setPen(QPen(colEdge, 1, Qt::SolidLine, Qt::FlatCap));
			painter.setBrush(QBrush(col));
		}

		drawDot(ToCanvasCoordinates(_currentCurve.dots[i]), &painter);
	}
}

void CurveWidget::mouseReleaseEvent(QMouseEvent *event)
{
	isMouseDragging = 0;
	dragDotIdx = -1;
}

void CurveWidget::mouseMoveEvent(QMouseEvent *event)
{
	mousePosition = event->pos();

	if (isMouseDragging == 1)
	{
		centerOffset = dragStartPixelsOffset + (event->pos() - dragStartMousePos);
		repaint();
	}
	else if (isMouseDragging == 2)
	{
		if (dragDotIdx >= 0)
		{
			auto newPos = ToAnalyticCoordinates(mousePosition);
			if (_currentCurve.dots[dragDotIdx] != newPos)
			{
				_currentCurve.dots[dragDotIdx] = newPos;
				repaint();
			}
		}
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
	centerOffset = QPointF(50, 950);
	pixelsInUnitScale = 900.0f;
}


void CurveWidget::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::MouseButton::MiddleButton)
	{
		isMouseDragging = 1;
		dragStartMousePos = event->pos();
		dragStartPixelsOffset = centerOffset;
	}
	else if (event->button() == Qt::MouseButton::LeftButton)
	{
		isMouseDragging = 2;
		dragStartMousePos = event->pos();
		for(int i = 0; i < 4; i++)
		{
			if (mouseUnderPoint[i])
			{
				dragDotIdx = i;
				break;
			}
		}
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


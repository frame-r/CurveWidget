#ifndef CURVEWIDGET_H
#define CURVEWIDGET_H

#include <assert.h>
#include <algorithm> // max

#include <QWidget>
#include <QtGui>
#include <QtCore>
#include <QPoint>

struct CurveSegment
{
	QPointF q0;
	QPointF q1;
};

struct CurvePoint
{
	QPointF pos;
};

struct Curve
{
	std::vector<QPointF> points;

	int SugmentsNum() const
	{
		return std::max(0, (int)points.size() - 1);
	}
	CurveSegment FetchSegment(int num) const
	{
		assert(0 <= num && num < SugmentsNum());
		return CurveSegment{points[num], points[num + 1]};
	}

	int PointsNum() { return points.size(); }
	CurvePoint FetchPoint(int num)
	{
		assert(0 <= num && num < PointsNum());
		return CurvePoint{points[num]};
	}
};


class CurveWidget : public QWidget
{
	Q_OBJECT

	// Number of pixels in one analytic unit
	float pixelsInUnitScale{100};

	// Offset analytic coordinate system relative to Top Left widget point in pixels
	QPointF centerOffset;

	QPoint mousePosition;

	// Dragging data
	int isMouseDragging{0};
	QPoint dragStartMousePos;
	QPointF dragStartPixelsOffset;

	int mouseUnderDot{-1};
	int mouseUnderSeg{-1};

	std::unique_ptr<QTimer> _timer;

	Curve _currentCurve;

public:
	explicit CurveWidget(QWidget *parent = nullptr);

private slots:
	void ShowContextMenu(const QPoint &pos);
	void _OnTimer();

protected:
	void paintEvent(QPaintEvent *e) override;
	void mousePressEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void wheelEvent(QWheelEvent *event) override;
	void resizeEvent(QResizeEvent *event) override;

public:
	void SetCurve(const Curve& curve) { _currentCurve = curve; }
	void NormalizeView();

private:

	// Analytic Coordinate System	- The coordinate system in which the coordinates of the curve are specified
	// Canvas Coordinate System		- Coordinates inside the widget. Top left - (0, 0)

	//
	QPoint ToCanvasCoordinates(const QPointF& analyticPos);

	//
	QPointF ToAnalyticCoordinates(const QPoint& canvasPos);

	float GetAnalyticUnitInPixels() { return pixelsInUnitScale; }
	float GetPixelInAnalyticUnit() { return 1 / pixelsInUnitScale; }


};

#endif // CURVEWIDGET_H

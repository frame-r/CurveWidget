#ifndef CURVEWIDGET_H
#define CURVEWIDGET_H

#include <assert.h>
#include <algorithm> // max

#include <QWidget>
#include <QtGui>
#include <QtCore>
#include <QPoint>


struct Curve
{
	union
	{
		struct
		{
			QPointF A;
			QPointF B;
			QPointF P1;
			QPointF P2;
		};
		QPointF dots[4] = {QPointF(0.0f, 0.0f), QPointF(1.0f, 1.0f), QPointF(0.0f, 1.0f), QPointF(1.0f, 0.0f)};
	};

	Curve(){}

	QPointF Evaluate(float t)
	{
		return	1.0f * pow(t, 3.0f) * (B + 3.0f * (P1 - P2) - A) +
				3.0f * pow(t, 2.0f) * (A - 2.0f * P1 + P2) +
				3.0f * t * (P1 - A) +
				A;
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

	int mouseUnderPoint[4] = {};

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
	QPointF ToAnalyticCoordinates(const QPoint& canvasPos);

	float GetAnalyticUnitInPixels() { return pixelsInUnitScale; }
	float GetPixelInAnalyticUnit() { return 1 / pixelsInUnitScale; }
};

#endif // CURVEWIDGET_H

#ifndef CURVEWIDGET_H
#define CURVEWIDGET_H

#include <QWidget>
#include <QtGui>
#include <QtCore>
#include <QPoint>

class CurveWidget : public QWidget
{
	Q_OBJECT
public:
	explicit CurveWidget(QWidget *parent = nullptr);

signals:

public slots:

protected:

	void paintEvent(QPaintEvent *e) override;
	void mousePressEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void wheelEvent(QWheelEvent *event) override;
	void resizeEvent(QResizeEvent *event) override;


public:

	void NormalizeView();

private:

	// Analytic Coordinate System	- The coordinate system in which the coordinates of the curve are specified
	// Canvas Coordinate System		- Coordinates inside the widget. Top left - (0, 0)

	//
	QPoint ToCanvasCoordinates(const QPointF& analyticPos);

	//
	QPointF ToAnalyticCoordinates(const QPoint& canvasPos);

	float GetAnalyticUnitInPixels() { return scale; }
	float GetPixelInAnalyticUnit() { return 1 / scale; }

private:

	// Number of pixels in one analytic unit
	float scale{100};

	// Offset analytic coordinate system relative to Top Left widget point in pixels
	QPointF pixelsOffset;

	// Dragging temp varibales
	int dragging{0};
	QPoint startMousePos;
	QPointF startPixelsOffset;
};

#endif // CURVEWIDGET_H

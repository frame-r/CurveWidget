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

private:
	float GetLinesUnit();
	void NormalizeView();
	QPoint ToCanvasCoordinates(const QPoint& pos);
	QPoint ToAnalyticCoordinates(const QPoint& pos);

private:
	float scale{100};
	int dragging{0};
	QPoint startMousePos;
	QPoint startPixelsOffset;
	QPoint pixelsOffset;

};

#endif // CURVEWIDGET_H

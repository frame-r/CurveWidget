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

private:
	int getLinesUnit();
	void NormalizeView();
	QPoint ToCanvasCoordinates(const QPoint& pos);
	QPoint ToAnalyticCoordinates(const QPoint& pos);

private:
	int scale{100};
	QPoint pixelsOffset;

};

#endif // CURVEWIDGET_H

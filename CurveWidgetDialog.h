#ifndef CURVEWIDGETDIALOG_H
#define CURVEWIDGETDIALOG_H

#include <QDialog>

namespace Ui {
class CurveWidgetDialog;
}

class CurveWidgetDialog : public QDialog
{
	Q_OBJECT

public:
	explicit CurveWidgetDialog(QWidget *parent = 0);
	~CurveWidgetDialog();

private slots:

private:
	Ui::CurveWidgetDialog *ui;
};

#endif // CURVEWIDGETDIALOG_H

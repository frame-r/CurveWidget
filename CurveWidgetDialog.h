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
	void on_pushButton_clicked();

	void on_pushButton_2_clicked();

private:
	Ui::CurveWidgetDialog *ui;
};

#endif // CURVEWIDGETDIALOG_H

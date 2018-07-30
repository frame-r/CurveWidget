#include "CurveWidgetDialog.h"
#include "ui_CurveWidgetDialog.h"
#include <QDesktopWidget>
#include <QRect>

CurveWidgetDialog::CurveWidgetDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::CurveWidgetDialog)
{
	ui->setupUi(this);

	//setWindowFlags(Qt::Tool);
	//setModal(true);

	QDesktopWidget *desktop = QApplication::desktop();
	QRect screenSize = desktop->availableGeometry(this);
	resize(QSize(screenSize.width() * 0.5f, screenSize.height() * 0.5f));
}

CurveWidgetDialog::~CurveWidgetDialog()
{
	delete ui;
}

void CurveWidgetDialog::on_pushButton_clicked()
{
	close();
}

void CurveWidgetDialog::on_pushButton_2_clicked()
{
	close();
}

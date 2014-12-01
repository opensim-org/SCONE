#ifndef SCONESTUDIO_H
#define SCONESTUDIO_H

#include <QtGui/QMainWindow>
#include "ui_SconeStudio.h"

class SconeStudio : public QMainWindow
{
	Q_OBJECT

public:
	SconeStudio(QWidget *parent = 0, Qt::WFlags flags = 0);
	~SconeStudio();

private:
	Ui::SconeStudioClass ui;
};

#endif // SCONESTUDIO_H

#ifndef SCONESTUDIO_H
#define SCONESTUDIO_H

#include <QtWidgets/QMainWindow>
#include "ui_SconeStudio.h"
#include "../core/PropNode.h"
#include "SconeManager.h"

class QFileSystemModel;

class SconeStudio : public QMainWindow
{
	Q_OBJECT

public:
	SconeStudio(QWidget *parent = 0, Qt::WindowFlags flags = 0);
	~SconeStudio();

	bool init();

public slots:
	void activateBrowserItem( QModelIndex idx );

private:
	scone::SconeManager m_Manager;
	Ui::SconeStudioClass ui;
	QFileSystemModel *m_pFileModel;
};

#endif // SCONESTUDIO_H

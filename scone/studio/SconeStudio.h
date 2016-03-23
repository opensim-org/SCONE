#ifndef SCONESTUDIO_H
#define SCONESTUDIO_H


#include <QtCore/QtGlobal>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    #include <QtWidgets/QMainWindow>
#else
    #include <QtGui/QMainWindow>
    namespace Qt { typedef WFlags WindowFlags; }
#endif
#include "ui_SconeStudio.h"
#include "scone/core/PropNode.h"
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

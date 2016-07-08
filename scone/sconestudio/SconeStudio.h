#ifndef SCONESTUDIO_H
#define SCONESTUDIO_H

#include <QtCore/QtGlobal>
#include <QtCore/QTimer>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    #include <QtWidgets/QMainWindow>
#else
    #include <QtGui/QMainWindow>
    namespace Qt { typedef WFlags WindowFlags; }
#endif

#include "ui_SconeStudio.h"
#include "scone/core/PropNode.h"

#include "SconeManager.h"
#include "StudioManager.h"
#include "flut/timer.hpp"
#include "flut/math/delta.hpp"

class QFileSystemModel;

using scone::TimeInSeconds;

class SconeStudio : public QMainWindow
{
	Q_OBJECT

public:
	SconeStudio(QWidget *parent = 0, Qt::WindowFlags flags = 0);
	~SconeStudio();

	bool init(osgViewer::ViewerBase::ThreadingModel threadingModel);

public slots:
	void activateBrowserItem( QModelIndex idx );
	void updateScrollbar( int pos );
	void start();
	void stop();
	void slomo( int v );
	void updateTimer();

private:
	void setTime( TimeInSeconds t );
	scone::StudioManager manager;
	Ui::SconeStudioClass ui;

	QFileSystemModel *m_pFileModel;
	QTimer qtimer;

	double slomo_factor;
	TimeInSeconds current_time;
	flut::timer timer;
	flut::delta< TimeInSeconds > timer_delta;
	flut::delta< scone::Vec3 > com_delta;
};

#endif // SCONESTUDIO_H

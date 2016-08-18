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
#include "StudioScene.h"
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
	void updateSpinBox( double );
	void start();
	void stop();
	void slomo( int v );
	void speed1() { slomo( 1 ); }
	void speed4() { slomo( 4 ); }
	void speed16() { slomo( 16 ); }
	void updateTimer();

private:
	void setTime( TimeInSeconds t );
	scone::StudioScene manager;
	Ui::SconeStudioClass ui;

	QFileSystemModel *resultsFileModel;
	QFileSystemModel *scenarioFileModel;
	QTimer qtimer;

	double slomo_factor;
	TimeInSeconds current_time;
	flut::timer timer;
	flut::delta< TimeInSeconds > timer_delta;
	flut::delta< scone::Vec3 > com_delta;
};

#endif // SCONESTUDIO_H

#ifndef SCONESTUDIO_H
#define SCONESTUDIO_H

#include <QtCore/QtGlobal>
#include <QtCore/QTimer>
#include <QtCore/QProcess>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    #include <QtWidgets/QMainWindow>
#else
    #include <QtGui/QMainWindow>
    namespace Qt { typedef WFlags WindowFlags; }
#endif

#include "ui_SconeStudio.h"

#include "scone/core/PropNode.h"
#include "scone/core/Statistic.h"

#include "SconeManager.h"
#include "StudioScene.h"
#include "flut/timer.hpp"
#include "flut/math/delta.hpp"
#include "ProgressDockWidget.h"
#include "QCodeEditor.h"
#include "Settings.h"
#include "flut/system/log_sink.hpp"

class QFileSystemModel;

using scone::TimeInSeconds;

class SconeStudio : public QMainWindow, public flut::log::sink
{
	Q_OBJECT

public:
	SconeStudio(QWidget *parent = 0, Qt::WindowFlags flags = 0);
	~SconeStudio();

	bool init(osgViewer::ViewerBase::ThreadingModel threadingModel);
	virtual void send_log_message( flut::log::level l, const std::string& msg ) override;

	public slots:

	void activateBrowserItem( QModelIndex idx );
	void selectBrowserItem( const QModelIndex& idx, const QModelIndex& idxold );

	void resultsSelectionChanged( const QItemSelection& newitem, const QItemSelection& olditem ) {}
	void updateScrollbar( int pos ) { setTime( double( pos ) / 1000 ); }
	void updateSpinBox( double value ) { setTime( value ); }
	void start();
	void stop();
	void next() { if ( !isPlaying() ) setTime( current_time + 0.01 ); }
	void previous() { if ( !isPlaying() ) setTime( current_time - 0.01 ); }
	void slomo( int v );
	void updateTimer();
	void fileOpen();
	void fileOpenRecent();
	void fileSave();
	void fileSaveAs();
	void fileExit();
	void showViewer() { ui.tabWidget->setCurrentIndex( 0 ); }
	void helpAbout() {}
	void optimizeScenario();
	void optimizeScenarioMultiple();
	void abortOptimizations();
	void updateBackgroundTimer();
	void updateOptimizations();
	void createVideo();
	void tabCloseRequested( int idx );
	void updateViewSettings();
	void showSettingsDialog() { settings.showDialog( this ); }

public:
	bool close_all;
	bool isRecording() { return !captureFilename.isEmpty(); }
	bool isEvalutating() { return manager.HasModel() && manager.GetModel().IsEvaluating(); }
	bool isPlaying() { return qtimer.isActive(); }

private:
	void setTime( TimeInSeconds t );
	QCodeEditor* getActiveScenario();
	bool checkAndSaveScenario( QCodeEditor* s );
	void addProgressDock( ProgressDockWidget* pdw );
	void addRecentFile( QString& filename );
	void updateRecentFilesMenu();
	scone::StudioScene manager;
	Ui::SconeStudioClass ui;

	QTimer qtimer;
	QTimer backgroundUpdateTimer;
	bool fileChanged = false;

	double slomo_factor;
	TimeInSeconds current_time;
	TimeInSeconds capture_frequency;
	TimeInSeconds evaluation_time_step;
	flut::timer timer;
	flut::delta< TimeInSeconds > timer_delta;
	flut::delta< scone::Vec3 > com_delta;

	std::vector< ProgressDockWidget* > optimizations;
	std::vector< QCodeEditor* > scenarios;

	QString captureFilename;
	QProcess* captureProcess;
	QDir captureImageDir;
	void finalizeCapture();

	scone::Settings settings;
	bool enableLogging;

protected:
	virtual void closeEvent( QCloseEvent * ) override;
};

#endif // SCONESTUDIO_H

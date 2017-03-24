#ifndef SCONESTUDIO_H
#define SCONESTUDIO_H

#include <QtCore/QtGlobal>
#include <QtCore/QTimer>
#include <QtCore/QProcess>
#include <QtWidgets/QMainWindow>

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
#include "QCompositeMainWindow.h"
#include "QDataAnalysisView.h"

using scone::TimeInSeconds;

class SconeStudio : public QCompositeMainWindow
{
	Q_OBJECT

public:
	SconeStudio(QWidget *parent = 0, Qt::WindowFlags flags = 0);
	~SconeStudio();

	bool init(osgViewer::ViewerBase::ThreadingModel threadingModel);

public slots:
	void activateBrowserItem( QModelIndex idx );
	void selectBrowserItem( const QModelIndex& idx, const QModelIndex& idxold );

	void resultsSelectionChanged( const QItemSelection& newitem, const QItemSelection& olditem ) {}
	void start();
	void stop();
	void refreshAnalysis();
	void fileOpen();
	void fileOpen( const QString& filename );
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
	void viewResults( bool v ) { if ( v ) ui.resultsDock->show(); else ui.resultsDock->hide(); }
	void viewMessages( bool v ) { if ( v ) ui.messagesDock->show(); else ui.messagesDock->hide(); }
	void fixViewCheckboxes();

public:
	bool close_all;
	bool isRecording() { return !captureFilename.isEmpty(); }
	bool isEvalutating() { return manager.HasModel() && manager.GetModel().IsEvaluating(); }

private:
	void evaluate();
	void setTime( TimeInSeconds t );
	QCodeEditor* getActiveScenario();
	bool checkAndSaveScenario( QCodeEditor* s );
	void addProgressDock( ProgressDockWidget* pdw );
	void addRecentFile( const QString& filename );
	void updateRecentFilesMenu();
	QStringList recentFiles;

	scone::StudioScene manager;
	Ui::SconeStudioClass ui;

	QTimer backgroundUpdateTimer;
	bool fileChanged = false;

	double slomo_factor;
	TimeInSeconds current_time;
	TimeInSeconds capture_frequency;
	TimeInSeconds evaluation_time_step;
	flut::delta< scone::Vec3 > com_delta;

	std::vector< ProgressDockWidget* > optimizations;
	std::vector< QCodeEditor* > scenarios;

	SconeStorageDataModel storageModel;
	QDataAnalysisView* analysisView;

	QString captureFilename;
	QProcess* captureProcess;
	QDir captureImageDir;
	void finalizeCapture();

	scone::Settings settings;

protected:
	virtual void closeEvent( QCloseEvent * ) override;
};

#endif // SCONESTUDIO_H

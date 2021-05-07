/*
** SconeStudio.h
**
** Copyright (C) 2013-2021 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#ifndef SCONESTUDIO_H
#define SCONESTUDIO_H

#include <QtCore/QtGlobal>
#include <QtCore/QTimer>
#include <QtCore/QProcess>
#include <QtWidgets/QMainWindow>
#include "QCodeEditor.h"
#include "QCompositeMainWindow.h"
#include "QDataAnalysisView.h"
#include "QGroup.h"
#include "QValueSlider.h"
#include "QDockWidget"
#include "QPropNodeItemModel.h"

#include "ui_SconeStudio.h"

#include "scone/core/PropNode.h"
#include "scone/core/Statistic.h"

#include "ModelVis.h"
#include "GaitAnalysis.h"
#include "ProgressDockWidget.h"
#include "ResultsFileSystemModel.h"
#include "SconeStorageDataModel.h"
#include "SettingsEditor.h"
#include "StudioModel.h"

#include "vis/plane.h"
#include "vis/vis_api.h"
#include "vis/scene.h"

#include "xo/container/flat_map.h"
#include "xo/numerical/delta.h"
#include "xo/system/log_sink.h"
#include "xo/time/timer.h"
#include "GaitAnalysis.h"
#include "ParTableModel.h"

using scone::TimeInSeconds;

class SconeStudio : public QCompositeMainWindow
{
	Q_OBJECT

public:
	SconeStudio(QWidget *parent = 0, Qt::WindowFlags flags = 0);
	~SconeStudio();

	bool init();
	virtual void openFile( const QString& filename ) override;
	virtual bool tryExit() override;

public slots:
	void activateBrowserItem( QModelIndex idx );
	void selectBrowserItem( const QModelIndex& idx, const QModelIndex& idxold );
	void resultsSelectionChanged( const QItemSelection& newitem, const QItemSelection& olditem ) {}
	void start();
	void stop();
	void refreshAnalysis();

	virtual void fileOpenTriggered() override;
	virtual void fileReloadTriggered();
	virtual void fileSaveTriggered() override;
	virtual void fileSaveAsTriggered() override;
	virtual void fileCloseTriggered() override;

	void helpSearch();
	void helpForum();
	void helpAbout();
	void evaluateActiveScenario();
	void performanceTestNormal() { performanceTest( false ); }
	void performanceTestWriteStats() { performanceTest( true ); }
	void optimizeScenario();
	void optimizeScenarioMultiple();
	bool abortOptimizations();
	void updateBackgroundTimer();
	void updateOptimizations();
	void createVideo();
	void captureImage();
	void modelAnalysis();
	void muscleAnalysis();
	void updateGaitAnalysis();
	void activateAnalysisFilter();
	void tabCloseRequested( int idx );
	void updateViewSettings();
	void showSettingsDialog();
	void setPlaybackTime( TimeInSeconds t ) { setTime( t, true ); }
	void updateTabTitles();
	void findDialog() { if ( auto* e = getActiveCodeEditor() ) e->findDialog(); }
	void findNext() { if ( auto* e = getActiveCodeEditor() ) e->findNext(); }
	void findPrevious() { if ( auto* e = getActiveCodeEditor() ) e->findNext( true ); }
	void toggleComments() { if ( auto* e = getActiveCodeEditor() ) e->toggleComments(); }
	void resetWindowLayout();

	void deleteSelectedFileOrFolder();
	void sortResultsByDate();
	void sortResultsByName();
	void onResultBrowserCustomContextMenu(const QPoint &);

public:
	bool close_all;
	bool isRecording() { return !captureFilename.isEmpty(); }
	bool isEvalutating() { return scenario_ && scenario_->IsEvaluating(); }

private:
	QCodeEditor* getActiveCodeEditor();
	QCodeEditor* getActiveScenario();
	void restoreCustomSettings( QSettings& settings ) override;
	void saveCustomSettings( QSettings& settings ) override;

	void performanceTest( bool write_stats );
	void evaluate();
	void setTime( TimeInSeconds t, bool update_vis );
	std::vector< QCodeEditor* > changedDocuments();
	bool requestSaveChanges( const std::vector<QCodeEditor*>& modified_docs );
	bool requestSaveChanges( QCodeEditor* s );
	int getTabIndex( QCodeEditor* s );
	void addProgressDock( ProgressDockWidget* pdw );

	// ui
	Ui::SconeStudioClass ui;
	scone::SettingsEditor settings;

	// model
	std::unique_ptr< scone::StudioModel > scenario_;
	bool createScenario( const QString& any_file );
	bool createAndVerifyActiveScenario( bool always_create );
	void updateModelDataWidgets();

	// simulation
	TimeInSeconds current_time;
	TimeInSeconds evaluation_time_step;

	// scenario
	std::vector< ProgressDockWidget* > optimizations;
	ResultsFileSystemModel* resultsModel;
	std::vector< QCodeEditor* > codeEditors;

	// viewer
	xo::flat_map< scone::ModelVis::VisOpt, QAction* > viewActions;
	vis::scene scene_;
	QTimer backgroundUpdateTimer;
	double slomo_factor;
	xo::delta< scone::Vec3 > com_delta;

	// video capture
	QString captureFilename;
	QProcess* captureProcess;
	QDir captureImageDir;
	void finalizeCapture();

	// analysis
	SconeStorageDataModel analysisStorageModel;
	QDataAnalysisView* analysisView;
	QDockWidget* analysisDock;

	// gait analysis
	scone::GaitAnalysis* gaitAnalysis;
	QDockWidget* gaitAnalysisDock;

	// parameters
	QTableView* parView;
	ParTableModel* parModel;
	QDockWidget* parViewDock;

	// results
	QTreeView* reportView;
	QPropNodeItemModel* reportModel;
	QDockWidget* reportDock;

	//// dof editor
	//QFormGroup* dofSliderGroup;
	//std::vector< QValueSlider* > dofSliders;
};

#endif // SCONESTUDIO_H

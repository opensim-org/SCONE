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
#include "ui_SconeProgressDockWidget.h"

#include <BasicXMLSyntaxHighlighter/BasicXMLSyntaxHighlighter.h>

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
	void selectBrowserItem( const QModelIndex& idx, const QModelIndex& idxold );
	void resultsSelectionChanged( const QItemSelection& newitem, const QItemSelection& olditem );
	void updateScrollbar( int pos );
	void updateSpinBox( double );
	void start();
	void stop();
	void slomo( int v );
	void speed1() { slomo( 1 ); }
	void speed4() { slomo( 4 ); }
	void speed16() { slomo( 16 ); }
	void updateTimer();
	void fileOpen();
	void fileSave();
	void fileSaveAs();
	void fileExit();
	void showEditor() { ui.stackedWidget->setCurrentIndex( 1 ); }
	void showViewer() { ui.stackedWidget->setCurrentIndex( 0 ); }
	void helpAbout() {}
	void optimizeScenario();
	void abortOptimizations();
	void updateOptimizations();

private:
	void setTime( TimeInSeconds t );
	scone::StudioScene manager;
	Ui::SconeStudioClass ui;
	BasicXMLSyntaxHighlighter* xmlSyntaxHighlighter;

	QFileSystemModel* resultsFileModel;
	QFileSystemModel* scenarioFileModel;
	QTimer qtimer;
	QTimer optimizationUpdateTimer;

	QString currentFileName;
	bool fileChanged = false;

	double slomo_factor;
	TimeInSeconds current_time;
	flut::timer timer;
	flut::delta< TimeInSeconds > timer_delta;
	flut::delta< scone::Vec3 > com_delta;

	struct Optimization
	{
		Optimization() : process( nullptr ), generation( 0 ), max_generations( 0 ), best( 0.0f ), best_gen( 0 ) {}
		~Optimization() {}
		
		scone::String name;
		QString fileName;
		QProcess* process;
		Ui::SconeProgressDockWidget* dock_ui;
		QDockWidget* dock;
		int best_gen;
		float best;
		float cur_best;
		float cur_avg;
		int generation;
		int max_generations;
		std::vector< float > bestvec;
		std::vector< float > avgvec;
	};

	std::vector< Optimization > optimizations;
protected:
	virtual void closeEvent( QCloseEvent * ) override;
};

#endif // SCONESTUDIO_H

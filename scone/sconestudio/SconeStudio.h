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

#include <BasicXMLSyntaxHighlighter/BasicXMLSyntaxHighlighter.h>

#include "scone/core/PropNode.h"
#include "scone/core/Statistic.h"

#include "SconeManager.h"
#include "StudioScene.h"
#include "flut/timer.hpp"
#include "flut/math/delta.hpp"
#include "ProgressDockWidget.h"

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

public:
	bool close_all;

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

	std::vector< ProgressDockWidget* > optimizations;
protected:
	virtual void closeEvent( QCloseEvent * ) override;
};

#endif // SCONESTUDIO_H

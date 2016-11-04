#include "ProgressDockWidget.h"
#include "SconeStudio.h"
#include "qt_tools.h"
#include "flut/system_tools.hpp"
#include "flut/system/assert.hpp"
#include "scone/core/Log.h"
#include "flut/string_tools.hpp"
#include "studio_config.h"

using namespace scone;

ProgressDockWidget::ProgressDockWidget( SconeStudio* s, const QString& config_file, const QStringList& extra_args ) :
studio( s ),
process( nullptr ),
generation( 0 ),
max_generations( 0 ),
best( 0.0f ),
best_gen( 0 ),
highest( 0 ),
lowest( 0 ),
state( StartingState )
{
	QString program = make_qt( flut::get_application_folder() / SCONE_SCONECMD_EXECUTABLE );
	QStringList args;
	args << "-o" << config_file << "-s" << "-q" << "-l" << "7" << extra_args;
	//for ( auto& a : args ) log::trace( a.toStdString() );

	process = new QProcess( this );
	process->setReadChannel( QProcess::StandardOutput );
	process->start( program, args );
	fileName = config_file;

	flut_error_if( !process->waitForStarted( 5000 ), "Could not start scenario " + fileName.toStdString() );
	scone::log::info( "Started scenario ", fileName.toStdString() );

	ui.setupUi( this );

	ui.plot->addGraph();
	ui.plot->graph(0)->setPen(QPen(QColor(0, 100, 255)));
	ui.plot->graph(0)->setLineStyle(QCPGraph::lsLine);
	//opt.ui.plot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 5));
	ui.plot->graph(0)->setName("Best fitness");

	ui.plot->addGraph();
	ui.plot->graph(1)->setPen(QPen(QColor(255, 100, 0), 1, Qt::DashLine ));
	ui.plot->graph(1)->setLineStyle(QCPGraph::lsLine);
	//opt.ui.plot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 5));
	ui.plot->graph(1)->setName("Average fitness");

	ui.plot->xAxis->setAutoTickCount( 6 );
	ui.plot->yAxis->setAutoTickCount( 3 );
	ui.plot->replot();
	ui.plot->hide();

	updateText();
}

ProgressDockWidget::~ProgressDockWidget()
{
	if ( state != ClosedState )
	{
		log::critical( "Deleting Progress Dock that is not closed: ", name.toStdString() );
		if ( process )
			delete process;
	}
}

ProgressDockWidget::UpdateResult ProgressDockWidget::updateProgress()
{
	SCONE_ASSERT( process );

	if ( !process->isOpen() )
	{
		log::trace( "process is closed" );
		close();
		return IsClosedResult;
	}

	if ( state == StartingState )
	{
		if ( process->waitForReadyRead( 1000 ) )
			state = InitializingState;
		else return OkResult;
	}

	while ( process->canReadLine() )
	{
		std::string s = flut::trim_str( QString::fromLocal8Bit( process->readLine() ).toStdString() );
		if ( !s.empty() ) log::trace( "MESSAGE: ", s );

		if ( s.empty() || s[ 0 ] != '*' )
			continue; // this is no message for us

		auto kvp = flut::to_key_value( s.substr( 1 ) );
		if ( kvp.first == "folder" )
		{
			name = make_qt( flut::get_filename_without_folder( flut::left_str( kvp.second, -1 ) ) );
			state = RunningState;
			ui.plot->show();
			setWindowTitle( name );
			log::debug( "Initialized optimization ", name.toStdString() );
		}
		else if ( kvp.first == "max_generations" )
		{
			max_generations = flut::from_str< int >( kvp.second );
			updateText();
		}
		else if ( kvp.first == "generation" )
		{
			flut::scan_str( kvp.second, generation, cur_avg, cur_best );
			avgvec.push_back( cur_avg );
			bestvec.push_back( cur_best );
			genvec.push_back( generation );
			highest = std::max( highest, std::max( cur_best, cur_avg ) );
			lowest = std::min( lowest, std::min( cur_best, cur_avg ) );
			updateText();
			ui.plot->graph( 0 )->setData( genvec, bestvec );
			ui.plot->graph( 1 )->setData( genvec, avgvec );
			ui.plot->xAxis->setRange( 0, generation );
			ui.plot->yAxis->setRange( lowest, highest );
			ui.plot->replot();
		}
		else if ( kvp.first == "best" )
		{
			best = flut::from_str< float >( kvp.second );
			best_gen = generation;
			updateText();
		}
		else if ( kvp.first == "error" )
		{
			errorMsg = make_qt( kvp.second );
			state = ErrorState;
			updateText();
			log::error( "Error optimizing ", fileName.toStdString(), ": ", errorMsg.toStdString() );
			return ShowErrorResult;
		}
		else if ( kvp.first == "finished" )
		{
			state = FinishedState;
			updateText();
		}
		else
		{
			// if this key has a value, keep it and display it as a tooltip
			if ( !kvp.second.empty() )
			{
				tooltipText += make_qt( ( tooltipText.isEmpty() ? "" : "\n" ) + kvp.first + " = " + kvp.second );
				ui.text->setToolTip( tooltipText );
			}
		}
	}

	return OkResult;
}

bool ProgressDockWidget::readyForDestruction()
{
	return state == ClosedState;
}

void ProgressDockWidget::closeEvent( QCloseEvent *e )
{
	if ( !studio->close_all && !( state == FinishedState || state == ErrorState ) )
	{
		// allow user to cancel close
		QString message = "Are you sure you want to abort optimization " + name;
		if ( QMessageBox::warning( this, "Abort Optimization", message, QMessageBox::Abort, QMessageBox::Cancel ) == QMessageBox::Cancel )
		{
			e->ignore();
			return;
		}
	}

	process->close();
	delete process;
	process = nullptr;

	state = ClosedState;

	e->accept();
}

void ProgressDockWidget::updateText()
{
	QString s;

	switch ( state )
	{
	case ProgressDockWidget::StartingState:
		s = "Initializing optimization...";
		break;
	case ProgressDockWidget::RunningState: 
		s = QString().sprintf( "Generation %d of %d. Best=%.3f (Gen %d)", generation, max_generations, best, best_gen );
		break;
	case ProgressDockWidget::FinishedState:
		s = QString().sprintf( "Optimization finished. Best=%.3f (Gen %d)", best, best_gen );
		break;
	case ProgressDockWidget::ClosedState:
		break;
	case ProgressDockWidget::ErrorState:
		s = errorMsg;
		break;
	default:
		break;
	}
	ui.text->setText( s );
}

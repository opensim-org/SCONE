#include "ProgressDockWidget.h"
#include "SconeStudio.h"
#include "qt_tools.h"
#include "flut/system_tools.hpp"
#include "flut/system/assert.hpp"
#include "scone/core/Log.h"
#include "flut/string_tools.hpp"

using namespace scone;

#ifdef _MSC_VER
	const char* scone_program_name = "sconecmd.exe";
#else
	const char* scone_program_name = "sconecmd";
#endif

ProgressDockWidget::ProgressDockWidget( SconeStudio* s, const QString& config_file ) :
studio( s ),
process( nullptr ),
generation( 0 ),
max_generations( 0 ),
best( 0.0f ),
best_gen( 0 ),
highest( 0 ),
lowest( 0 )
{
	QString program = make_qt( flut::get_application_folder() + scone_program_name );
	QStringList args;
	args << "-o" << config_file << "-s" << "-q";
	process = new QProcess( this );
	process->setReadChannel( QProcess::StandardOutput );
	process->start( program, args );
	fileName = config_file;

	flut_error_if( !process->waitForStarted( 5000 ), "Could not start process" );
	scone::log::info( "Started optimization of ", fileName.toStdString() );

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

}

ProgressDockWidget::~ProgressDockWidget()
{
	if ( process )
		delete process;
}

bool ProgressDockWidget::updateProgress()
{
	SCONE_ASSERT( process );

	if ( !process->isOpen() )
	{
		log::trace( "process is closed" );
		close();
		return false;
	}

	if ( name.empty() )
		process->waitForReadyRead( 5000 );

	while ( process->canReadLine() )
	{
		std::string s = QString::fromLocal8Bit( process->readLine() ).toStdString();
		auto kvp = flut::to_key_value( s );
		//log::trace( flut::quoted( kvp.first ), " = ", flut::quoted( kvp.second ) );

		if ( kvp.first == "folder" )
		{
			name = flut::get_filename_without_folder( flut::left_str( kvp.second, -1 ) );
			setWindowTitle( QString( name.c_str() ) );
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
			QMessageBox::critical( this, "Error optimizing " + fileName, make_qt( kvp.second ) );
		}
		else if ( kvp.first == "finished" )
		{
			QMessageBox::information( this, "Optimization finished", fileName + " is finished" );
		}
	}

	return true;
}

bool ProgressDockWidget::isFinished()
{
	return !name.empty() && isHidden();
}

void ProgressDockWidget::closeEvent( QCloseEvent *e )
{
	if ( !studio->close_all && process->isOpen() )
	{
		QString message = QString( "Are you sure you want to abort optimization " ) + QString( name.c_str() );
		if ( QMessageBox::warning( this, "Abort Optimization", message, QMessageBox::Abort, QMessageBox::Cancel ) == QMessageBox::Cancel )
		{
			e->ignore();
			return;
		}
	}

	scone::log::info( "Closing process ", name );
	process->close();
	delete process;
	process = nullptr;
	e->accept();
}

void ProgressDockWidget::updateText()
{
	QString s = QString().sprintf( "Generation %d of %d; best=%.3f (Generation %d)", generation + 1, max_generations, best, best_gen );
	ui.text->setText( s );
}

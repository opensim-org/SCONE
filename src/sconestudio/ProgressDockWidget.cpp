#include "ProgressDockWidget.h"
#include "SconeStudio.h"
#include "qt_tools.h"
#include "xo/system/system_tools.h"
#include "xo/system/assert.h"
#include "scone/core/Log.h"
#include "xo/string/string_tools.h"
#include "studio_config.h"
#include "xo/filesystem/filesystem.h"

using namespace scone;

ProgressDockWidget::ProgressDockWidget( SconeStudio* s, const QString& config_file, const QStringList& extra_args ) :
studio( s ),
process( nullptr ),
cur_gen( 0 ),
max_generations( 0 ),
best( 0.0f ),
best_gen( 0 ),
highest( 0 ),
lowest( 0 ),
cur_pred( 0 ),
state( StartingState ),
view_first_gen( 0 )
{
	QString program = make_qt( xo::get_application_folder() / SCONE_SCONECMD_EXECUTABLE );
	QStringList args;
	args << "-o" << config_file << "-s" << "-q" << "-l" << "7" << extra_args;

	process = new QProcess( this );
	process->setReadChannel( QProcess::StandardOutput );
	process->start( program, args );
	fileName = config_file;

	xo_error_if( !process->waitForStarted( 5000 ), "Could not start scenario " + fileName.toStdString() );
	scone::log::info( "Started scenario ", fileName.toStdString() );

	ui.setupUi( this );

	//ui.plot->xAxis->setLabel( "Generation" );
	ui.plot->setInteraction( QCP::iRangeZoom, true );
	ui.plot->axisRect()->setRangeZoom( Qt::Horizontal );
	ui.plot->setInteraction( QCP::iRangeDrag, true );
	ui.plot->axisRect()->setRangeDrag( Qt::Horizontal );
	ui.plot->xAxis->setLabelPadding( 0 );
	ui.plot->xAxis->setTickLabelPadding( 2 );
	ui.plot->yAxis->setLabel( "Fitness" );
	ui.plot->yAxis->setLabelPadding( 1 );
	ui.plot->yAxis->setTickLabelPadding( 2 );
	ui.plot->setContentsMargins( 0, 0, 0, 0 );
	ui.plot->xAxis->marginSideToAxisType( QCP::msNone );
	QFont font = ui.plot->font();
	font.setPointSize( 7 );
	ui.plot->xAxis->setTickLabelFont( font );
	ui.plot->yAxis->setTickLabelFont( font );

	ui.plot->addGraph();
	ui.plot->graph( 0 )->setPen( QPen( QColor( 0, 100, 255 ) ) );
	ui.plot->graph( 0 )->setLineStyle( QCPGraph::lsLine );
	ui.plot->graph( 0 )->setName( "Best fitness" );
	//opt.ui.plot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 5));

	ui.plot->addGraph();
	ui.plot->graph( 1 )->setPen( QPen( QColor( 255, 100, 0 ), 1, Qt::DashLine ) );
	ui.plot->graph( 1 )->setLineStyle( QCPGraph::lsLine );
	ui.plot->graph( 1 )->setName( "Average fitness" );
	//opt.ui.plot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 5));

	ui.plot->addGraph();
	ui.plot->graph( 2 )->setPen( QPen( QColor( 50, 50, 50 ), 1, Qt::SolidLine ) );
	ui.plot->graph( 2 )->setLineStyle( QCPGraph::lsLine );
	ui.plot->graph( 2 )->setName( "Trend" );
	//opt.ui.plot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 5));

	ui.plot->xAxis->setRange( 0, min_view_gens );
	ui.plot->xAxis->setAutoTickCount( 6 );
	ui.plot->yAxis->setAutoTickCount( 3 );
	ui.plot->replot();
	ui.plot->hide();

	connect( ui.plot->xAxis, SIGNAL( rangeChanged( const QCPRange&, const QCPRange& ) ), this, SLOT( rangeChanged( const QCPRange&, const QCPRange& ) ) );

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

void ProgressDockWidget::SetAxisScaleType( AxisScaleType ast, double log_base )
{
	//log::info( "Changing axis style to ", ast );
	//ui.plot->yAxis->setScaleType( Linear ? QCPAxis::stLinear : QCPAxis::stLogarithmic );
	//ui.plot->yAxis->setScaleLogBase( log_base );
}

void ProgressDockWidget::rangeChanged( const QCPRange &newRange, const QCPRange &oldRange )
{
	view_first_gen = xo::clamped( static_cast<int>( newRange.lower ), 0, xo::max( cur_gen - min_view_gens, 0 ) );
	view_last_gen = xo::clamped( static_cast<int>( newRange.upper ), min_view_gens, xo::max( cur_gen, min_view_gens ) );

	ui.plot->xAxis->blockSignals( true );
	log::info( "Fixing X range to ", view_first_gen, " - ", view_last_gen );
	ui.plot->xAxis->setRange( view_first_gen, view_last_gen );
	ui.plot->xAxis->blockSignals( false );

	auto bestminmax = std::minmax_element( bestvec.begin() + view_first_gen, bestvec.end() );
	ui.plot->yAxis->setRange( *bestminmax.first, *bestminmax.second );
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
		std::string s = xo::trim_str( QString::fromLocal8Bit( process->readLine() ).toStdString() );
		//if ( !s.empty() ) log::trace( name, ": ", s );

		if ( s.empty() || s[ 0 ] != '*' )
			continue; // this is no message for us

		auto kvp = xo::make_key_value_str( s.substr( 1 ) );
		if ( kvp.first == "folder" )
		{
			name = make_qt( path( kvp.second ).filename() );
			state = RunningState;
			ui.plot->show();
			setWindowTitle( name );
			log::debug( "Initialized optimization ", name.toStdString() );
		}
		else if ( kvp.first == "max_generations" )
		{
			max_generations = xo::from_str< int >( kvp.second );
			updateText();
		}
		else if ( kvp.first == "window_size" )
		{
			window_size = xo::from_str< int >( kvp.second );
		}
		else if ( kvp.first == "generation" )
		{
			xo::scan_str( kvp.second, cur_gen, cur_best, cur_med, cur_avg, cur_reg[ 0 ], cur_reg[ 1 ] );
			avgvec.push_back( cur_avg );
			bestvec.push_back( cur_best );
			medvec.push_back( cur_med );
			genvec.push_back( cur_gen );
			highest = std::max( highest, std::max( cur_best, cur_avg ) );
			lowest = std::min( lowest, std::min( cur_best, cur_avg ) );
			cur_pred = cur_reg( float( max_generations ) );
			updateText();

			ui.plot->graph( 0 )->setData( genvec, bestvec );
			ui.plot->graph( 1 )->setData( genvec, avgvec );
			ui.plot->graph( 2 )->clearData();
			auto start = std::max( 0, cur_gen - window_size );
			ui.plot->graph( 2 )->addData( start, cur_reg( start ) );
			ui.plot->graph( 2 )->addData( cur_gen, cur_reg( float( cur_gen ) ) );

			ui.plot->xAxis->setRange( view_first_gen, cur_gen );
			ui.plot->replot();
		}
		else if ( kvp.first == "best" )
		{
			best = xo::from_str< float >( kvp.second );
			best_gen = cur_gen;
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
			errorMsg = make_qt( kvp.second );
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
		s = QString().sprintf( "Gen %d; Best=%.3f (Gen %d); P=%.3f", cur_gen, best, best_gen, cur_pred );
		break;
	case ProgressDockWidget::FinishedState:
		s = QString().sprintf( "Finished (Gen %d); Best=%.3f (Gen %d)", cur_gen, best, best_gen ) + "\n" + errorMsg;
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

#include "ProgressDockWidget.h"
#include "SconeStudio.h"
#include "qt_tools.h"
#include "xo/system/system_tools.h"
#include "xo/system/assert.h"
#include "scone/core/Log.h"
#include "xo/string/string_tools.h"
#include "studio_config.h"
#include "xo/filesystem/filesystem.h"
#include "xo/serialization/prop_node_serializer_zml.h"

using namespace scone;

ProgressDockWidget::ProgressDockWidget( SconeStudio* s, const QString& config_file, const QStringList& extra_args ) :
studio( s ),
process( nullptr ),
state( StartingState ),
min_view_gens( 3 ),
view_first_gen( 0 ),
view_last_gen( min_view_gens )
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
		log::critical( "Deleting Progress Dock that is not closed: ", getIdentifier().toStdString() );
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
	auto it = std::max_element( optimizations.begin(), optimizations.end(), [&]( auto& a, auto& b ) { return a.cur_gen < b.cur_gen; } );

	view_first_gen = xo::clamped( static_cast<int>( newRange.lower ), 0, xo::max( it->cur_gen - min_view_gens, 0 ) );
	view_last_gen = xo::clamped( static_cast<int>( newRange.upper ), min_view_gens, xo::max( it->cur_gen, min_view_gens ) );

	//log::info( "setting x-range to ", view_first_gen, " ", view_last_gen );
	ui.plot->xAxis->blockSignals( true );
	ui.plot->xAxis->setRange( view_first_gen, view_last_gen );
	ui.plot->xAxis->blockSignals( false );

	fixRangeY();
}

void ProgressDockWidget::fixRangeY()
{
	double upper = 0.0, lower = 0.0;
	for ( auto& o : optimizations )
	{
		if ( view_first_gen < o.bestvec.size() )
		{
			auto bestminmax = std::minmax_element( o.bestvec.begin() + view_first_gen, o.bestvec.end() );
			xo::set_if_smaller( lower, *bestminmax.first );
			xo::set_if_bigger( upper, *bestminmax.second );
		}
	}
	//log::info( "setting y-range to ", lower, " ", upper );
	ui.plot->yAxis->setRange( lower, upper );
}

void ProgressDockWidget::Optimization::Update( const PropNode& pn, ProgressDockWidget& wdg )
{
	if ( pn.try_get( max_generations, "max_generations" ) )
		wdg.updateText();

	pn.try_get( window_size, "window_size" );

	if ( pn.try_get( cur_gen, "step" ) )
	{
		pn.try_get( cur_reg.offset(), "trend_offset" );
		pn.try_get( cur_reg.slope(), "trend_slope" );
		avgvec.push_back( pn.get< double >( "step_average" ) );
		bestvec.push_back( pn.get< double >( "step_best" ) );
		genvec.push_back( cur_gen );
		cur_pred = cur_reg( float( cur_gen + window_size ) );
	}

	if ( pn.try_get( best, "best" ) )
	{
		best_gen = cur_gen;
		wdg.updateText();
	}

	has_update_flag = true;
}

ProgressDockWidget::ProgressResult ProgressDockWidget::updateProgress()
{
	SCONE_ASSERT( process );

	if ( !process->isOpen() )
	{
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
		char buf[ 1024 ];
		process->readLine( buf, 1023 );
		string s( buf );
		//log::info( s );

		if ( s.empty() || s[ 0 ] != '*' )
			continue; // this is no message for us

		std::stringstream str( s.substr( 1 ) );
		xo::prop_node pn;
		xo::error_code ec;
		str >> xo::prop_node_serializer_zml( pn, &ec );

		if ( ec.bad() )
		{
			log::warning( "Error parsing message: ", s );
			continue;
		}

		if ( auto id = pn.try_get< string >( "id" ) )
		{
			auto it = xo::find_if( optimizations, [&]( auto& o ) { return o.name == *id; } );
			if ( it == optimizations.end() )
			{
				Optimization new_opt;
				auto idx = optimizations.size();

				new_opt.idx = idx;
				new_opt.name = *id;
				new_opt.Update( pn, *this );

				// add graphs
				QColor c = make_qt( vis::make_unique_color( idx ) );
#ifdef SCONE_SHOW_TREND_LINES
				ui.plot->addGraph();
				ui.plot->graph( idx * 2 )->setPen( QPen( c ) );
				ui.plot->graph( idx * 2 )->setLineStyle( QCPGraph::lsLine );
				ui.plot->addGraph();
				ui.plot->graph( idx * 2 + 1 )->setPen( QPen( c.lighter(), 1, Qt::DashLine ) );
				ui.plot->graph( idx * 2 + 1 )->setLineStyle( QCPGraph::lsLine );
#else
				ui.plot->addGraph();
				ui.plot->graph( idx )->setPen( QPen( c ) );
				ui.plot->graph( idx )->setLineStyle( QCPGraph::lsLine );
#endif
				ui.plot->show();

				optimizations.push_back( std::move( new_opt ) );
				setWindowTitle( make_qt( *id ) );
				log::info( "Initialized optimization ", *id );

				state = RunningState;
			}
			else
			{
				it->Update( pn, *this );
				updateText();
				tooltipProps.merge( pn );
			}

		}

		if ( pn.try_get( errorMsg, "error" ) )
		{
			state = ErrorState;
			updateText();
			log::error( "Error optimizing ", fileName.toStdString(), ": ", errorMsg.toStdString() );
			return ShowErrorResult;
		}
		else if ( pn.try_get( errorMsg, "finished" ) )
		{
			state = FinishedState;
			updateText();
		}
	}

	for ( index_t idx = 0; idx < optimizations.size(); ++idx )
	{
		auto& o = optimizations[ idx ];
		if ( o.has_update_flag )
		{
			o.has_update_flag = false;

			// update graphs
#ifdef SCONE_SHOW_TREND_LINES
			ui.plot->graph( idx * 2 )->setData( o.genvec, o.bestvec );
			auto start_gen = std::max( 0, o.cur_gen - o.window_size );
			ui.plot->graph( idx * 2 + 1 )->setData( QVector< double >{ start_gen, o.cur_gen }, QVector< double >{ o.cur_reg( start_gen ), o.cur_reg( float( o.cur_gen ) ); } );
#else
			ui.plot->graph( idx )->setData( o.genvec, o.bestvec );
#endif

			// update range and replot
			ui.plot->xAxis->setRange( view_first_gen, std::max( o.cur_gen, view_last_gen ) );
			fixRangeY();
			ui.plot->replot();
		}
	}

	tooltipText = make_qt( to_str( tooltipProps ) );
	ui.text->setToolTip( tooltipText );

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
		QString message = "Are you sure you want to abort optimization " + getIdentifier();
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
		//s = QString().sprintf( "Gen %d; Best=%.3f (Gen %d); P=%.3f", cur_gen, best, best_gen, cur_pred );
		break;
	case ProgressDockWidget::FinishedState:
		//s = QString().sprintf( "Finished (Gen %d); Best=%.3f (Gen %d)", cur_gen, best, best_gen ) + "\n" + errorMsg;
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

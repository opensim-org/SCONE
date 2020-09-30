/*
** ProgressDockWidget.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "ProgressDockWidget.h"
#include "SconeStudio.h"
#include "xo/system/system_tools.h"
#include "xo/system/assert.h"
#include "scone/core/Log.h"
#include "scone/core/string_tools.h"
#include "studio_config.h"
#include "xo/filesystem/filesystem.h"
#include "xo/serialization/prop_node_serializer_zml.h"
#include "StudioSettings.h"
#include "xo/container/container_tools.h"
#include "xo/serialization/prop_node_serializer_ini.h"
#include <sstream>

using namespace scone;

ProgressDockWidget::ProgressDockWidget( SconeStudio* s, std::unique_ptr<scone::OptimizerTask> t ) :
	studio( s ),
	task_( std::move( t ) ),
	state( StartingState ),
	showCloseWarning( true ),
	closeWhenFinished( false ),
	min_view_gens( 20 ),
	view_first_gen( 0 ),
	view_last_gen( min_view_gens ),
	best_idx( -1 )
{
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
	ui.plot->setContentsMargins( 1, 1, 1, 1 );
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
		log::critical( "Deleting Progress Dock that is not closed: ", getIdentifier().toStdString() );
	else log::debug( "Closed optimization ", getIdentifier().toStdString() );
}

void ProgressDockWidget::SetAxisScaleType( AxisScaleType ast, double log_base )
{
	//log::info( "Changing axis style to ", ast );
	//ui.plot->yAxis->setScaleType( Linear ? QCPAxis::stLinear : QCPAxis::stLogarithmic );
	//ui.plot->yAxis->setScaleLogBase( log_base );
}

void ProgressDockWidget::rangeChanged( const QCPRange& newRange, const QCPRange& oldRange )
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

void ProgressDockWidget::Optimization::Update( const PropNode& pn )
{
	//log::debug( "Messsage:\n", pn );
	pn.try_get( max_generations, "max_generations" );
	pn.try_get( window_size, "window_size" );
	pn.try_get( is_minimizing, "minimize" );

	if ( pn.try_get( cur_gen, "step" ) )
	{
		pn.try_get( cur_reg.offset(), "trend_offset" );
		pn.try_get( cur_reg.slope(), "trend_slope" );
		medvec.push_back( pn.get< double >( "step_median" ) );
		bestvec.push_back( pn.get< double >( "step_best" ) );
		genvec.push_back( cur_gen );
		if ( !pn.try_get( cur_pred, "predicted_fitness" ) )
			cur_pred = cur_reg( float( cur_gen + window_size ) );
	}

	pn.try_get( best, "best" );
	pn.try_get( best_gen, "best_gen" );
	pn.try_get( duration, "time" );

	if ( pn.try_get( message, "finished" ) )
	{
		state = FinishedState;
		log::info( "Finished ", name, stringf( " (%.2fs)", duration ), ": ", message );
	}

	if ( pn.try_get( message, "error" ) )
	{
		state = ErrorState;
		log::error( "Optimization ", name, " error: ", message );
	}

	has_update_flag = true;
}

ProgressDockWidget::ProgressResult ProgressDockWidget::updateProgress()
{
	// check if there was a request to close and the optimizer is finished
	if ( closeWhenFinished && ( state == FinishedState || state == ErrorState ) )
	{
 		close();
 		return IsClosedResult;
	}

	for ( auto messages = task_->getMessages(); !messages.empty(); messages.pop_front() )
	{
		const auto& pn = messages.front();
		if ( auto id = pn.try_get< string >( "id" ) )
		{
			auto it = xo::find_if( optimizations, [&]( auto& o ) { return o.name == *id; } );
			if ( it == optimizations.end() )
			{
				Optimization new_opt;
				auto idx = optimizations.size();

				new_opt.idx = idx;
				new_opt.name = *id;
				new_opt.Update( pn );
				new_opt.state = RunningState;
				state = RunningState;

				// add graphs
				QColor c = to_qt( xo::make_unique_color( idx ) );
#ifdef SCONE_SHOW_TREND_LINES
				ui.plot->addGraph();
				ui.plot->graph( idx * 2 )->setPen( QPen( c, GetStudioSetting< float >( "progress.line_width" ) ) );
				ui.plot->graph( idx * 2 )->setLineStyle( QCPGraph::lsLine );
				ui.plot->addGraph();
				ui.plot->graph( idx * 2 + 1 )->setPen( QPen( c.lighter(), 1, Qt::DashLine ) );
				ui.plot->graph( idx * 2 + 1 )->setLineStyle( QCPGraph::lsLine );
#else
				ui.plot->addGraph();
				ui.plot->graph( idx )->setPen( QPen( c, GetStudioSetting< float >( "progress.line_width" ) ) );
				ui.plot->graph( idx )->setLineStyle( QCPGraph::lsLine );
#endif
				ui.plot->show();

				optimizations.push_back( std::move( new_opt ) );

				if ( scenario.empty() )
				{
					setWindowTitle( to_qt( scenario = *id ) );
					updateText();
				}

				log::debug( "Initialized optimization ", *id );
			}
			else
			{
				it->Update( pn );

				if ( optimizations.size() == 1 )
				{
					state = it->state;
					message = it->message;
				}

				if ( best_idx == -1 )
					best_idx = it->idx;
				else if ( ( it->is_minimizing && it->best < optimizations[ best_idx ].best ) || ( !it->is_minimizing && it->best > optimizations[ best_idx ].best ) )
					best_idx = it->idx;

				updateText();
			}
		}
		else // generic message
		{
			if ( pn.try_get( scenario, "scenario" ) )
				setWindowTitle( to_qt( scenario ) );

			if ( pn.try_get( message, "error" ) )
			{
				state = ErrorState;
				updateText();
				log::error( "Error optimizing ", task_->scenario_file_.toStdString(), ": ", message );
				return ShowErrorResult;
			}
			else if ( pn.try_get( message, "finished" ) )
			{
				state = FinishedState;
				updateText();
			}
		}

		// always merge data into tooltip
		tooltipProps.merge( pn, true );
	}

	auto new_view_last_gen = view_last_gen;
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
			auto* g = ui.plot->graph( idx );
			for ( int i = g->data()->size(); i < o.genvec.size(); ++i )
				g->addData( o.genvec[ i ], o.bestvec[ i ] );
			if ( o.genvec.size() >= view_last_gen )
				new_view_last_gen = o.genvec.size() - 1;
#endif
		}
	}

	// update range and replot
	if ( new_view_last_gen != view_last_gen || view_last_gen <= min_view_gens )
	{
		if ( view_first_gen > 0 )
			view_first_gen += new_view_last_gen - view_last_gen;
		view_last_gen = new_view_last_gen;
		ui.plot->xAxis->setRange( view_first_gen, view_last_gen );
		fixRangeY();
	}
	ui.plot->replot();

	// #todo: use to_str instead
	std::stringstream str;
	xo::prop_node_serializer_ini( tooltipProps ).write_stream( str );
	tooltipText = to_qt( str.str() );
	ui.text->setToolTip( tooltipText );

	return OkResult;
}

bool ProgressDockWidget::readyForDestruction() const
{
	return state == ClosedState;
}

bool ProgressDockWidget::canCloseWithoutWarning() const
{
	return !showCloseWarning || state == FinishedState || state == ErrorState;
}

void ProgressDockWidget::closeEvent( QCloseEvent* e )
{
	if ( !canCloseWithoutWarning() )
	{
		// allow user to cancel close
		QString message = "Are you sure you want to abort optimization " + getIdentifier();
		if ( QMessageBox::warning( this, "Abort Optimization", message, QMessageBox::Abort, QMessageBox::Cancel ) == QMessageBox::Cancel )
		{
			e->ignore();
			return;
		}
	}

	if ( state == StartingState || state == RunningState )
	{
		// send interrupt signal, window will close once done
		log::debug( "Canceling optimization ", getIdentifier().toStdString() );
		task_->interrupt();
		closeWhenFinished = true;
		updateText();
		e->ignore();
	}

	if ( state == FinishedState || state == ErrorState )
	{
		// task is done, close widget
		task_->finish();
		state = ClosedState;
		e->accept();
	}
}

void ProgressDockWidget::updateText()
{
	string s;

	auto* opt = ( best_idx != -1 ) ? &optimizations[ best_idx ] : nullptr;

	switch ( state )
	{
	case ProgressDockWidget::StartingState:
		s = "Initializing optimization...";
		break;
	case ProgressDockWidget::RunningState:
		if ( closeWhenFinished )
			s = "Canceling optimization...";
		else if ( opt )
			s = xo::stringf( "Gen %d; Best=%.3f (Gen %d); P=%.3f", opt->cur_gen, opt->best, opt->best_gen, opt->cur_pred );
		else s = "Waiting for first evaluation...";
		break;
	case ProgressDockWidget::FinishedState:
		if ( opt )
			s = xo::stringf( "Finished (Gen %d); Best=%.3f (Gen %d)", opt->cur_gen, opt->best, opt->best_gen ) + "\n" + message;
		else s = message;
		break;
	case ProgressDockWidget::ClosedState:
		break;
	case ProgressDockWidget::ErrorState:
		s = message;
		break;
	default:
		break;
	}
	ui.text->setText( to_qt( s ) );
}

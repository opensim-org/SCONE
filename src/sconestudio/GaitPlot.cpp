#include "GaitPlot.h"

#include "qcustomplot/qcustomplot.h"

#include "StudioSettings.h"
#include "scone/core/math.h"
#include "xo/container/flat_map.h"
#include "xo/container/prop_node_tools.h"
#include "xo/numerical/bounds.h"
#include "xo/numerical/interpolation.h"
#include "xo/numerical/math.h"
#include "xo/utility/frange.h"
#include "scone/core/Log.h"
#include "xo/container/container_tools.h"

namespace scone
{
	GaitPlot::GaitPlot( const PropNode& pn, QWidget* parent ) :
		QWidget( parent ),
		INIT_MEMBER( pn, title_, "" ),
		INIT_MEMBER_REQUIRED( pn, left_channel_ ),
		INIT_MEMBER_REQUIRED( pn, right_channel_ ),
		INIT_MEMBER_REQUIRED( pn, row_ ),
		INIT_MEMBER_REQUIRED( pn, column_ ),
		INIT_MEMBER( pn, x_label_, "x" ),
		INIT_MEMBER( pn, y_label_, "y" ),
		INIT_MEMBER( pn, y_min_, 0 ),
		INIT_MEMBER( pn, y_max_, 0 ),
		INIT_MEMBER( pn, channel_offset_, 0 ),
		INIT_MEMBER( pn, channel_multiply_, 1.0 ),
		INIT_MEMBER( pn, norm_offset_, 0 ),
		plot_( nullptr ),
		plot_title_( nullptr )
	{
		auto l = new QHBoxLayout( this );
		l->setContentsMargins( 0, 0, 0, 0 );

		plot_ = new QCustomPlot( this );
		l->addWidget( plot_ );

		// title
		plot_->plotLayout()->insertRow( 0 ); // inserts an empty row above the default axis rect
		plot_title_ = new QCPPlotTitle( plot_, title_.c_str() );
		plot_->plotLayout()->addElement( 0, 0, plot_title_ );

		// norm data
		auto norm_min = pn.try_get_child( "norm_min" );
		auto norm_max = pn.try_get_child( "norm_max" );
		if ( norm_min && norm_max )
		{
			if ( norm_min->size() == norm_max->size() )
			{
				auto* top = plot_->addGraph();
				auto* bot = plot_->addGraph();
				norm_data_.reserve( norm_min->size() );
				for ( index_t i = 0; i < norm_min->size(); ++i )
				{
					auto yt = norm_max->get<double>( i ) + norm_offset_;
					auto yb = norm_min->get<double>( i ) + norm_offset_;
					y_min_ = xo::min( y_min_, yb );
					y_max_ = xo::max( y_max_, yt );
					double x = 100.0 * i / ( norm_min->size() - 1 );
					top->addData( x, yt );
					bot->addData( x, yb );
					norm_data_[ x ] = { yb, yt };
				}
				top->setPen( Qt::NoPen );
				bot->setPen( Qt::NoPen );
				top->setBrush( QColor( 0, 0, 0, 30.0 ) );
				top->setChannelFillGraph( bot );
			}
			else log::warning( "Invalid norm data for ", title_, ", norm_min has ", norm_min->size(), " data points, norm_max has ", norm_max->size() );
		}

		// margins
		plot_->plotLayout()->setMargins( QMargins( 2, 2, 2, 2 ) );
		plot_->axisRect()->setMinimumMargins( QMargins( 1, 1, 1, 1 ) );
		plot_->yAxis->setLabelPadding( 2 );
		plot_->yAxis->setTickLabelPadding( 2 );
		plot_->xAxis->setLabelPadding( 2 );
		plot_->xAxis->setTickLabelPadding( 2 );

		// fonts
		auto labelFont = plot_->xAxis->labelFont();
		labelFont.setPointSize( 10 );
		plot_->xAxis->setLabelFont( labelFont );
		plot_->yAxis->setLabelFont( labelFont );

		// labels
		plot_->xAxis->setLabel( x_label_.c_str() );
		plot_->yAxis->setLabel( y_label_.c_str() );

		// set axes ranges, so we see all data:
		plot_->xAxis->setRange( 0, 100 );
		plot_->xAxis->setAutoTickStep( true );
		plot_->xAxis->setAutoSubTicks( false );
		plot_->xAxis->setAutoTickCount( 4 );
		plot_->yAxis->setRange( y_min_, y_max_ );
		plot_->yAxis->setAutoTickStep( true );
		plot_->yAxis->setAutoSubTicks( false );
		plot_->yAxis->setAutoTickCount( 4 );

		plot_->replot();
	}

	xo::error_message GaitPlot::update( const Storage<>& sto, const std::vector<GaitCycle>& cycles )
	{
		while ( plot_->graphCount() > 2 )
			plot_->removeGraph( plot_->graphCount() - 1 );

		// find channels, report error if not find
		const auto& labels = sto.GetLabels();
		auto right_channel_idx = xo::find_index_if( labels, [&]( auto& l ) { return right_channel_( l ); } );
		auto left_channel_idx = xo::find_index_if( labels, [&]( auto& l ) { return left_channel_( l ); } );
		if ( right_channel_idx == no_index && left_channel_idx == no_index )
			return "Could not find " + left_channel_.str() + " / " + right_channel_.str() + "; please verify Tools->Preferences->Data";

		xo::flat_map< double, double > avg_data;
		auto s = 1.0 / cycles.size();

		bool plot_cycles = GetStudioSetting<bool>( "gait_analysis.plot_individual_cycles" );

		for ( const auto& cycle : cycles )
		{
			bool right = cycle.side_ == RightSide;
			const auto& channel_name = right ? right_channel_ : left_channel_;
			auto channel_idx = right ? right_channel_idx : left_channel_idx;
			if ( channel_idx != no_index )
			{
				auto* graph = plot_cycles ? plot_->addGraph() : nullptr;
				if ( graph ) graph->setPen( QPen( right ? Qt::red : Qt::blue, 1 ) );
				for ( Real perc : xo::frange<Real>( 0.0, 100.0, 0.5 ) )
				{
					auto f = sto.GetInterpolatedFrame( cycle.begin_ + perc * cycle.duration() / 100.0 );
					auto value = channel_offset_ + channel_multiply_ * f.value( channel_idx );
					if ( graph ) graph->addData( perc, value );
					avg_data[ perc ] += s * value;
				}
			}
			else log::warning( "Gait Analysis could not find: ", channel_name ); // only shown when *either* left / right is missing
		}

		// add average data plot
		if ( !avg_data.empty() )
		{
			auto* avg_graph = plot_->addGraph();
			avg_graph->setPen( QPen( Qt::black, 1.5 ) );
			for ( auto& e : avg_data )
				avg_graph->addData( e.first, e.second );
		}

		// compute average error in STD
		if ( !avg_data.empty() && !norm_data_.empty() )
		{
			double error = 0.0;
			for ( const auto& [x, r] : norm_data_ )
				error += xo::abs( r.get_violation( xo::lerp_map( avg_data, x ) ) ) / xo::max( 0.01, r.range() );
			error /= norm_data_.size();
			auto fit_perc = 100.0 * xo::clamped( 1.0 - error, 0.0, 1.0 );
			if ( plot_title_ && GetStudioSetting<bool>( "gait_analysis.show_fit" ) )
				plot_title_->setText( title_.c_str() + QString::asprintf( " (%.1f%%)", fit_perc ) );
		}
		plot_->replot();

		return {};
	}
}

#include "GaitPlot.h"

#include "qcustomplot/qcustomplot.h"

#include "xo/container/prop_node_tools.h"
#include "xo/numerical/bounds.h"
#include "xo/numerical/math.h"
#include "scone/core/math.h"
#include "xo/utility/frange.h"
#include "xo/container/flat_map.h"

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
		INIT_MEMBER( pn, norm_offset_, 0 )
	{
		auto l = new QHBoxLayout( this );
		l->setContentsMargins( 0, 0, 0, 0 );

		plot_ = new QCustomPlot( this );
		l->addWidget( plot_ );

		// title
		if ( !title_.empty() )
		{
			plot_->plotLayout()->insertRow( 0 ); // inserts an empty row above the default axis rect
			plot_->plotLayout()->addElement( 0, 0, new QCPPlotTitle( plot_, title_.c_str() ) );
		}

		// norm data
		auto norm_min = pn.try_get_child( "norm_min" );
		auto norm_max = pn.try_get_child( "norm_max" );
		if ( norm_min && norm_max && norm_min->size() == norm_max->size() )
		{
			auto* top = plot_->addGraph();
			auto* bot = plot_->addGraph();
			for ( index_t i = 0; i < norm_min->size(); ++i )
			{
				auto yt = norm_max->get<double>( i ) + norm_offset_;
				auto yb = norm_min->get<double>( i ) + norm_offset_;
				y_min_ = xo::min( y_min_, yb );
				y_max_ = xo::max( y_max_, yt );
				double x = 100.0 * i / ( norm_min->size() - 1 );
				top->addData( x, yt );
				bot->addData( x, yb );
			}
			top->setPen( Qt::NoPen );
			bot->setPen( Qt::NoPen );
			top->setBrush( QColor( 0, 0, 0, 30.0 ) );
			top->setChannelFillGraph( bot );
		}

		// graph line
// 		auto* graph = plot_->addGraph();
// 		graph->setPen( QPen( Qt::black, 1.5 ) );

		// labels
		plot_->xAxis->setLabel( x_label_.c_str() );
		plot_->yAxis->setLabel( y_label_.c_str() );

		// set axes ranges, so we see all data:
		plot_->xAxis->setRange( 0, 100 );
		plot_->xAxis->setAutoTickStep( true );
		plot_->xAxis->setAutoSubTicks( false );
		plot_->xAxis->setAutoTickCount( 4 );
		plot_->yAxis->setRange( y_min_, y_max_ );

		plot_->replot();
	}

	void GaitPlot::update( const Storage<>& sto, const std::vector<GaitCycle>& cycles )
	{
		while ( plot_->graphCount() > 2 )
			plot_->removeGraph( plot_->graphCount() - 1 );

		xo::flat_map< double, double > avg_data;
		auto s = 1.0 / cycles.size();

		for ( const auto& cycle : cycles )
		{
			bool right = cycle.side_ == RightSide;
			auto channel_idx = sto.GetChannelIndex( right ? right_channel_ : left_channel_ );
			if ( channel_idx != no_index )
			{
				auto* graph = plot_->addGraph();
				graph->setPen( QPen( right ? Qt::red : Qt::blue, 1 ) );

				for ( Real perc : xo::frange<Real>( 0.0, 100.0, 0.5 ) )
				{
					auto f = sto.GetInterpolatedFrame( cycle.begin_ + perc * cycle.duration() / 100.0 );
					auto value = channel_offset_ + channel_multiply_ * f.value( channel_idx );
					graph->addData( perc, value );
					avg_data[ perc ] += s * value;
				}
			}
		}

		// add average data plot
		auto* avg_graph = plot_->addGraph();
		avg_graph->setPen( QPen( Qt::black, 1.5 ) );
		for ( auto& e : avg_data )
			avg_graph->addData( e.first, e.second );

		plot_->replot();
	}
}

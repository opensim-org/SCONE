#include "GaitAnalysis.h"
#include "scone/core/storage_tools.h"
#include "scone/core/StorageIo.h"
#include "xo/filesystem/path.h"
#include "scone/core/Log.h"

#include "qcustomplot/qcustomplot.h"
#include "scone/core/system_tools.h"
#include "GaitPlot.h"
#include "xo/serialization/serialize.h"

namespace scone
{
	GaitAnalysis::GaitAnalysis( QWidget* parent ) :
		QWidget( parent )
	{
		grid_ = new QGridLayout( this );

		auto file = GetFolder( SCONE_RESOURCE_FOLDER ) / "gaitanalysis/default.zml";
		auto plot_pn = xo::load_file( file );
		for ( const auto& pn : plot_pn )
		{
			auto plot = new GaitPlot( pn.second );
			grid_->addWidget( plot, plot->row_, plot->column_ );
			plots_.push_back( plot );
		}
	}

	void GaitAnalysis::createChart( const String& channel, int row, int col )
	{
		auto channel_idx = sto_.GetChannelIndex( channel );
		SCONE_ERROR_IF( channel_idx == no_index, "Could not find " + channel );

		QCustomPlot* customPlot = new QCustomPlot();
		customPlot->plotLayout()->insertRow( 0 ); // inserts an empty row above the default axis rect
		customPlot->plotLayout()->addElement( 0, 0, new QCPPlotTitle( customPlot, "Your Plot Title" ) );
		auto* graph1 = customPlot->addGraph();
		auto* top = customPlot->addGraph();
		auto* bot = customPlot->addGraph();
		for ( const auto& f : sto_.GetData() )
		{
			graph1->addData( f->GetTime(), f->GetValues()[ channel_idx ] );
			top->addData( f->GetTime(), f->GetValues()[ channel_idx ] + 0.1 );
			bot->addData( f->GetTime(), f->GetValues()[ channel_idx ] - 0.2 );
		}

		// set area
		top->setPen( Qt::NoPen );
		top->setBrush( QColor( 0, 0, 0, 50 ) );
		top->setChannelFillGraph( bot );

		// give the axes some labels:
		customPlot->xAxis->setLabel( "x" );
		customPlot->yAxis->setLabel( "y" );

		// set axes ranges, so we see all data:
		customPlot->xAxis->setRange( 0, 100 );
		customPlot->xAxis->setAutoTickCount( 4 );
		customPlot->yAxis->setRange( -1, 1 );
		customPlot->replot();

		grid_->addWidget( customPlot, row, col );
	}

	void GaitAnalysis::update( const Storage<>& sto, const path& filename )
	{
		sto_ = ExtractGaitCycle( sto, "leg1_r.grf_norm_y" );
		WriteStorageTxt( sto_, filename + ".GaitCycle.txt", "" );
		log::info( "Results written to ", filename + ".GaitCycle.txt" );

		for ( auto* p : plots_ )
			p->update( sto_ );
	}
}

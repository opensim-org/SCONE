#include "GaitAnalysis.h"
#include "scone/core/storage_tools.h"
#include "scone/core/StorageIo.h"
#include "xo/filesystem/path.h"
#include "scone/core/Log.h"

#include "QPushButton"
#include "qcustomplot/qcustomplot.h"

namespace scone
{
	GaitAnalysis::GaitAnalysis( QWidget* parent ) :
		QWidget( parent )
	{
		grid_ = new QGridLayout( this );
	}

	void GaitAnalysis::createChart( const String& channel, int row, int col )
	{
		auto channel_idx = sto_.GetChannelIndex( channel );
		SCONE_ERROR_IF( channel_idx == no_index, "Could not find " + channel );

		QCustomPlot* customPlot = new QCustomPlot();
		customPlot->addGraph();
		for ( const auto& f : sto_.GetData() )
			customPlot->graph( 0 )->addData( f->GetTime(), f->GetValues()[ channel_idx ] );

		// give the axes some labels:
		customPlot->xAxis->setLabel( "x" );
		customPlot->yAxis->setLabel( "y" );
		// set axes ranges, so we see all data:
		customPlot->xAxis->setRange( 0, 100 );
		customPlot->yAxis->setRange( -1, 1 );
		customPlot->replot();

		grid_->addWidget( customPlot, row, col );
	}

	void GaitAnalysis::update( const Storage<>& sto, const path& filename )
	{
		sto_ = ExtractGaitCycle( sto, "leg1_r.grf_norm_y" );
		WriteStorageTxt( sto_, filename + ".GaitCycle.txt", "" );
		log::info( "Results written to ", filename + ".GaitCycle.txt" );

		createChart( "pelvis_tilt", 0, 0 );
		createChart( "hip_flexion_r", 0, 1 );
		createChart( "knee_angle_r", 1, 0 );
		createChart( "ankle_angle_r", 1, 1 );
	}
}

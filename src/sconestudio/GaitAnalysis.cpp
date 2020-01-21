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

	void GaitAnalysis::update( const Storage<>& sto, const path& filename )
	{
		sto_ = ExtractGaitCycle( sto, "leg1_r.grf_norm_y" );
		WriteStorageTxt( sto_, filename + ".GaitCycle.txt", "" );
		log::info( "Results written to ", filename + ".GaitCycle.txt" );

		for ( auto* p : plots_ )
			p->update( sto_ );
	}
}

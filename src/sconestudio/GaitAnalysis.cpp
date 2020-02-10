#include "GaitAnalysis.h"

#include "scone/core/storage_tools.h"
#include "scone/core/StorageIo.h"
#include "scone/core/system_tools.h"
#include "scone/core/Log.h"

#include "xo/filesystem/path.h"

#include "StudioSettings.h"
#include "GaitPlot.h"
#include "xo/serialization/serialize.h"

#include "qcustomplot/qcustomplot.h"
#include "scone/core/GaitCycle.h"
#include "xo/container/container_algorithms.h"

namespace scone
{
	GaitAnalysis::GaitAnalysis( QWidget* parent ) :
		QWidget( parent ),
		threshold_( 0.01 ),
		info_( "Gait Analysis" )
	{
		grid_ = new QGridLayout( this );
		grid_->setContentsMargins( 0, 0, 0, 0 );
		grid_->setSpacing( 0 );

		auto plot_pn = xo::load_file( GetStudioSetting<path>( "gait_analysis.templates" ) / "default.zml" );
		for ( const auto& pn : plot_pn )
		{
			auto plot = new GaitPlot( pn.second );
			grid_->addWidget( plot, plot->row_, plot->column_ );
			plots_.push_back( plot );
		}
	}

	void GaitAnalysis::update( const Storage<>& sto, const path& filename )
	{
		auto cycles = ExtractGaitCycles( sto, threshold_, 0.2 );

		if ( cycles.size() > 2 )
		{
			auto f = 1.0 / ( cycles.size() - 2 );
			auto avg_length = f * std::accumulate( cycles.begin() + 2, cycles.end(), 0.0,
				[]( const auto& v, const auto& c ) { return v + c.length();  } );
			auto avg_dur = f * std::accumulate( cycles.begin() + 2, cycles.end(), 0.0,
				[]( const auto& v, const auto& c ) { return v + c.duration();  } );
			info_.sprintf( "Gait Analysis - steps=%d length=%.2fm time=%.2fs", cycles.size(), avg_length, avg_dur );
		}

		for ( auto* p : plots_ )
			p->update( sto, cycles );
	}
}

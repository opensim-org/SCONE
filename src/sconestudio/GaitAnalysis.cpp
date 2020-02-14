#include "GaitAnalysis.h"

#include "GaitPlot.h"
#include "StudioSettings.h"
#include "qcustomplot/qcustomplot.h"
#include "scone/core/GaitCycle.h"
#include "scone/core/Log.h"
#include "scone/core/StorageIo.h"
#include "scone/core/storage_tools.h"
#include "scone/core/system_tools.h"
#include "xo/container/container_algorithms.h"
#include "xo/filesystem/path.h"
#include "xo/serialization/serialize.h"

namespace scone
{
	GaitAnalysis::GaitAnalysis( QWidget* parent ) :
		QWidget( parent ),
		threshold_( 0.01 ),
		skip_first_( 2 ),
		skip_last_( 1 ),
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
		if ( cycles.size() > skip_first_ + skip_last_ )
		{
			cycles.erase( cycles.begin(), cycles.begin() + skip_first_ );
			cycles.erase( cycles.end() - skip_last_, cycles.end() );

			for ( auto* p : plots_ )
				p->update( sto, cycles );

			auto f = 1.0 / cycles.size();
			auto avg_length = f * std::accumulate( cycles.begin(), cycles.end(), 0.0,
				[]( const auto& v, const auto& c ) { return v + c.length();  } );
			auto avg_dur = f * std::accumulate( cycles.begin(), cycles.end(), 0.0,
				[]( const auto& v, const auto& c ) { return v + c.duration();  } );
			info_.sprintf( "Gait Analysis - N=%d LEN=%.2fm DUR=%.2fs", cycles.size(), avg_length, avg_dur );
		}
		else log::warning( "Could not extract enough gait cycles from " + filename.str() );
	}
}

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
#include "scone/core/Settings.h"

namespace scone
{
	GaitAnalysis::GaitAnalysis( QWidget* parent ) :
		QWidget( parent ),
		grid_( nullptr )
	{
		reset();
	}

	void GaitAnalysis::reset()
	{
		if ( grid_ )
			delete grid_;
		plots_.clear();
		info_ = "Gait Analysis";

		grid_ = new QGridLayout( this );
		grid_->setContentsMargins( 0, 0, 0, 0 );
		grid_->setSpacing( 0 );

		xo::error_code ec;
		auto plot_pn = xo::load_file( GetStudioSetting<path>( "gait_analysis.template" ), &ec );
		if ( ec.good() )
		{
			for ( const auto& pn : plot_pn )
			{
				auto plot = new GaitPlot( pn.second );
				grid_->addWidget( plot, plot->row_, plot->column_ );
				plots_.push_back( plot );
			}
		}
		else log::error( "Error loading gait analysis template: ", ec.message() );
	}

	void GaitAnalysis::update( const Storage<>& sto, const path& filename )
	{
		auto force_threshold = GetStudioSetting<Real>( "gait_analysis.force_threshold" );
		auto min_duration = GetStudioSetting<Real>( "gait_analysis.min_stance_duration" );
		auto skip_first = GetStudioSetting<int>( "gait_analysis.skip_first" );
		auto skip_last = GetStudioSetting<int>( "gait_analysis.skip_last" );
		auto skip_total = skip_first + skip_last;
		auto cycles = ExtractGaitCycles( sto, force_threshold, min_duration );

		if ( cycles.size() > skip_total )
		{
			cycles.erase( cycles.begin(), cycles.begin() + skip_first );
			cycles.erase( cycles.end() - skip_last, cycles.end() );

			for ( auto* p : plots_ )
				p->update( sto, cycles );

			auto f = 1.0 / cycles.size();
			auto avg_length = f * std::accumulate( cycles.begin(), cycles.end(), 0.0,
				[]( const auto& v, const auto& c ) { return v + c.length();  } );
			auto avg_dur = f * std::accumulate( cycles.begin(), cycles.end(), 0.0,
				[]( const auto& v, const auto& c ) { return v + c.duration();  } );
			auto avg_speed = avg_length / avg_dur;
			info_ = QString::asprintf( "Gait Analysis - %zu steps; %.2fm; %.2fs; %0.2fm/s", cycles.size(), avg_length, avg_dur, avg_speed );
		}
		else log::warning( "Could not extract enough gait cycles from " + filename.str() );
	}
}

#include "MimicMeasure.h"

#include "../core/StorageIo.h"
#include "../model/Model.h"
#include "xo/numerical/math.h"

namespace scone
{
	
	MimicMeasure::MimicMeasure( const PropNode& props, Params& par, Model& model, const Locality& area ) :
	Measure( props, par, model, area )
	{
		INIT_PROP_REQUIRED( props, file_ );
		INIT_PROP( props, include_states_, xo::pattern_matcher( "*" ) );
		INIT_PROP( props, exclude_states_, xo::pattern_matcher( "" ) );
		INIT_PROP( props, use_best_match_, false );

		ReadStorageSto( storage_, FindFile( file_ ) );

		auto& s = model.GetState();
		for ( index_t state_idx = 0; state_idx < s.GetSize(); ++state_idx )
		{
			auto& name = s.GetName( state_idx );
			if ( include_states_( name ) && !exclude_states_( name ) )
			{
				index_t sto_idx = storage_.GetChannelIndex( name );
				if ( sto_idx != NoIndex )
				{
					state_storage_map_.emplace_back( state_idx, sto_idx );
					channel_errors_.emplace_back( name, 0.0 );
				}
			}
		}
		SCONE_THROW_IF( state_storage_map_.empty(), "No matching states found in " + file_.string() );

		model.AddExternalResource( file_ );
	}

	bool MimicMeasure::UpdateMeasure( const Model& model, double timestamp )
	{
		// when using a full motion, terminate when there's no more data
		if ( !use_best_match_ && timestamp > storage_.Back().GetTime() )
			return true;

		auto& s = model.GetState();
		double error = 0.0;
		index_t error_idx = 0;
		for ( auto& m : state_storage_map_ )
		{
			auto e = fabs( s[ m.first ] - storage_.GetInterpolatedValue( timestamp, m.second ) );
			channel_errors_[ error_idx++ ].second = e;
			error += e;
		}

		result_.AddSample( timestamp, error / state_storage_map_.size() );

		return false;
	}

	double MimicMeasure::GetResult( Model& model )
	{
		return use_best_match_ ? result_.GetLowest() : result_.GetAverage();
	}

	void MimicMeasure::StoreData( Storage<Real>::Frame& frame, const StoreDataFlags& flags ) const
	{
		frame[ "mimic_error" ] = result_.GetLatest();
		for ( auto& c : channel_errors_ )
			frame[ c.first + "_error" ] = c.second;
	}

	String MimicMeasure::GetClassSignature() const
	{
		return String( "M" );
	}
}

#include "MimicMeasure.h"

#include "../core/StorageIo.h"
#include "../model/Model.h"
#include "xo/numerical/math.h"

namespace scone
{
	
	MimicMeasure::MimicMeasure( const PropNode& props, Params& par, Model& model, const Locality& area ) :
	Measure( props, par, model, area )
	{
		INIT_PROP_REQUIRED( props, motion_file_ );
		ReadStorageSto( storage_, motion_file_ );
	}

	bool MimicMeasure::UpdateMeasure( const Model& model, double timestamp )
	{
		auto& s = model.GetState();

		double error = 0.0;
		for ( index_t state_idx = 0; state_idx < s.GetSize(); ++state_idx )
		{
			index_t sto_idx = storage_.GetChannelIndex( s.GetName( state_idx ) );
			if ( sto_idx != NoIndex )
				error += xo::squared( s[ state_idx ] - storage_.GetInterpolatedValue( timestamp, sto_idx ) );
		}

		result_.AddSample( timestamp, error );

		return false;
	}

	double MimicMeasure::GetResult( Model& model )
	{
		return sqrt( result_.GetAverage() );
	}

	String MimicMeasure::GetClassSignature() const
	{
		return String( "M" );
	}
}

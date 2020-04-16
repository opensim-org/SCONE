/*
** StepMeasure.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "StepMeasure.h"
#include "scone/model/Model.h"
#include "scone/core/Log.h"
#include "scone/core/Exception.h"
#include "scone/core/profiler_config.h"
#include "scone/core/GaitCycle.h"

namespace scone
{
	StepMeasure::StepMeasure( const PropNode& props, Params& par,
							  const Model& model, const Location& loc ) :
	Measure( props, par, model, loc )
	{
		INIT_PROP( props, stride_length, RangePenalty<Real>() );
		INIT_PROP( props, stride_duration, RangePenalty<Real>() );
		INIT_PROP( props, load_threshold, 0.01 );
		INIT_PROP( props, min_stance_duration_threshold, 0.2 );
		INIT_PROP( props, initiation_cycles, 1 );

		SCONE_THROW_IF(initiation_cycles < 1, "initiation_cycles should be >= 1");
		SCONE_THROW_IF(stride_length.IsNull() && stride_duration.IsNull() ,
					   "stride_length and/or stride_duration should be defined");
	}

	bool StepMeasure::UpdateMeasure( const Model& model, double timestamp )
	{
		SCONE_PROFILE_FUNCTION;

		auto& frame = stored_data_.AddFrame( timestamp );
		for ( const auto& leg : model.GetLegs() )
		{
			Vec3 force, moment, cop;
			leg->GetContactForceMomentCop( force, moment, cop );
			Vec3 grf = force / model.GetBW();

			frame[ leg->GetName() + ".grf_norm_x" ] = grf.x;
			frame[ leg->GetName() + ".grf_norm_y" ] = grf.y;
			frame[ leg->GetName() + ".grf_norm_z" ] = grf.z;
			frame[ leg->GetName() + ".cop_x" ] = cop.x;
			frame[ leg->GetName() + ".cop_y" ] = cop.y;
			frame[ leg->GetName() + ".cop_z" ] = cop.z;
		}

		return false;
	}

	double StepMeasure::ComputeResult( const Model& model )
	{
		auto cycles = ExtractGaitCycles( stored_data_,
										 load_threshold,
										 min_stance_duration_threshold );

		// calculate stride length or stride duration
		for (int cycle = initiation_cycles; cycle < cycles.size(); cycle++) {
			auto t = cycles[cycle].begin_;
			auto stride_dur = cycles[cycle].end_ - cycles[cycle].begin_;
			auto stride_len = cycles[cycle].length();

			if ( !stride_length.IsNull() )
				 stride_length.AddSample( t, stride_len );

			if ( !stride_duration.IsNull() )
				stride_duration.AddSample( t, stride_dur );
		}

		// calculate penalty
		double penalty = 0;
		if ( !stride_length.IsNull() )
		{
			penalty += stride_length.GetResult();
			GetReport().set( "stride_length_penalty" ,
							 stringf( "%g", stride_length.GetResult() ) );
		}

		if ( !stride_duration.IsNull() )
		{
			penalty += stride_duration.GetResult();
			GetReport().set( "stride_duration_penalty" ,
							 stringf( "%g", stride_duration.GetResult() ) );
		}

		return penalty;
	}

	String StepMeasure::GetClassSignature() const
	{
		return stringf( "SL" );
	}
}

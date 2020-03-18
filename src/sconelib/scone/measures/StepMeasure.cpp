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
// #include "xo/container/sorted_vector.h"
// #include "xo/container/container_algorithms.h"

namespace scone
{
	StepMeasure::StepMeasure( const PropNode& props, Params& par, const Model& model, const Location& loc ) :
	Measure( props, par, model, loc )
	{
		INIT_PROP( props, step_length, RangePenalty<Real>() );
		INIT_PROP( props, load_threshold, 0.2 );
		INIT_PROP( props, min_step_duration, 0.2 );
		INIT_PROP( props, initiation_steps, 2 );

		// check if step_length range penalty is defined by the user
		SCONE_THROW_IF(initiation_steps < 1, "The initiation_steps must be greater than zero.");

		// check if step_length range penalty is defined by the user
		SCONE_THROW_IF(step_length.IsNull(), "The step_length RangePenalty is not defined.");
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
		auto cycles = ExtractGaitCycles( stored_data_, load_threshold, min_step_duration );
		double penalty = 0;
		std::cout << "cycles: " << cycles.size() << std::endl;
		if (cycles.size() + 1 > initiation_steps)
		{
			// calculate step length
			int skip = initiation_steps;
			int cycle = 1;
			for (; cycle < cycles.size(); cycle++) {
				skip--;
				if (skip >= 0) continue;

				auto t = cycles[cycle - 1].begin_;
				auto step_len = xo::length(cycles[cycle].begin_pos_ - cycles[cycle - 1].begin_pos_);
				log::TraceF("%d %.3f @ %.3f", cycle, t, step_len );
				std::cout << t << std::endl;
				step_length.AddSample( t, step_len );
			}
			std::cout << "final one " << std::endl;
			// count the two steps of the final cycle
			auto c1 = cycles[cycle - 2];
			auto c2 = cycles[cycle - 1];
			if (skip < 0) // initiation_steps might be an odd number equal to the total steps - 1
			{
				auto t1 =  c2.begin_;
				std::cout << t1 << std::endl;
				auto step_len1 = xo::length(c1.end_pos_ - c2.begin_pos_);
				log::TraceF("f1 %.3f @ %.3f", t1, step_len1 );
				step_length.AddSample( t1, step_len1 );
			}
			std::cout << "final two " << std::endl;
			auto t2 = c1.end_;
			std::cout << t2 << std::endl;
			auto step_len2 = xo::length(c2.end_pos_ - c1.end_pos_);
			log::TraceF("f2 %.3f @ %.3f", t2, step_len2 );
			step_length.AddSample( t2, step_len2 );
			std::cout << "end " << std::endl;

			// calculate penalty
			if ( !step_length.IsNull() && step_length.GetNumSamples() > 0 )
			{
				penalty += step_length.GetResult();
				GetReport().set( "step_length_penalty" , stringf( "%g", step_length.GetResult() ) );
			}
		}

		return penalty;
	}

	String StepMeasure::GetClassSignature() const
	{
		return stringf( "L" );
	}

	void StepMeasure::StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const
	{
		if ( !step_length.IsNull() && step_length.GetNumSamples() > 0)
			frame[ GetName() + ".step_length_penalty" ] = step_length.GetLatest();
	}
}

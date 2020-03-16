/*
** StepLengthMeasure.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "StepLengthMeasure.h"
#include "scone/model/Model.h"
#include "scone/model/Body.h"
#include "scone/core/Log.h"
#include "scone/model/Muscle.h"
#include "scone/core/profiler_config.h"
#include "scone/core/Range.h"
#include "xo/container/sorted_vector.h"
#include "xo/container/container_algorithms.h"

namespace scone
{
	StepLengthMeasure::StepLengthMeasure( const PropNode& props, Params& par, const Model& model, const Location& loc ) :
	Measure( props, par, model, loc )
	{
		INIT_PROP( props, step_length, RangePenalty<Real>() );
		INIT_PROP( props, load_threshold, 0.1 );
		INIT_PROP( props, min_step_duration, 0.1 );
		INIT_PROP( props, initiation_steps, 2 );

		// # todo better handle step length exception
		if (step_length.IsNull())
			log::TraceF( "step_length not defined");

		INIT_PROP( props, upper_body, "torso" );
		INIT_PROP( props, base_bodies, "toes_l toes_r" );

		m_UpperBody = &( *FindByName( model.GetBodies(), upper_body ) );

		// extract individual body names from gait_bodies string
		auto tokens = xo::split_str( base_bodies, ";, " );
		for ( const String& t : tokens )
			m_BaseBodies.push_back( &( *FindByName( model.GetBodies(), t ) ) );

		m_InitGaitDist = m_PrevGaitDist = GetGaitDist( model );
		m_PrevTime = 0.0;
	}

	bool StepLengthMeasure::UpdateMeasure( const Model& model, double timestamp )
	{
		SCONE_PROFILE_FUNCTION;

		// make sure this is a new step
		SCONE_ASSERT( model.GetIntegrationStep() != model.GetPreviousIntegrationStep() );

		// update measure on new step
		bool new_contact = HasNewFootContact( model );
		TimeInSeconds dt = m_PrevTime > 0 ? timestamp : timestamp - m_PrevTime;
		if ( new_contact && dt > min_step_duration  && !step_length.IsNull() ) 
		{
			AddStep( model, timestamp );
			m_PrevTime = timestamp;
		}

		return false;
	}

	double StepLengthMeasure::ComputeResult( const Model& model )
	{
		// add final step and penalty to measure
		// #todo: only when not at the end of the simulation?
		AddStep( model, model.GetTime() );

		// calculate penalty
		double penalty = 0;
		if ( !step_length.IsNull() )
		{
			penalty += step_length.GetResult();
			GetReport().set( "step_length_penalty" , stringf( "%g", step_length.GetResult() ) );
		}

		return penalty;
	}

	void StepLengthMeasure::AddStep( const Model &model, double timestamp )
	{
		double gait_dist = GetGaitDist( model );
		step_length.AddSample( model.GetTime(), gait_dist - m_PrevGaitDist );
		m_PrevGaitDist = gait_dist;
	}

	scone::Real StepLengthMeasure::GetGaitDist( const Model &model )
	{
		// compute average of feet and Com (smallest 2 values)
		auto com_x = model.GetComPos().x;
		auto base1_x = m_BaseBodies[ 0 ]->GetComPos().x;
		auto base2_x = m_BaseBodies[ 1 ]->GetComPos().x;
#if 1
		xo::sorted_vector< double > distances{ com_x, base1_x, base2_x };
		return ( distances[ 0 ] + distances[ 1 ] ) / 2;
#else
		auto upper_x = m_UpperBody ? m_UpperBody->GetComPos().x : com_x;
		auto front_toe = xo::max( base1_x, base2_x );
		return xo::min( upper_x, front_toe );
#endif
	}

	String StepLengthMeasure::GetClassSignature() const
	{
		return stringf( "L" );
	}

	bool StepLengthMeasure::HasNewFootContact( const Model& model )
	{
		if ( m_PrevContactState.empty() )
		{
			// initialize
			for ( const LegUP& leg : model.GetLegs() )
				m_PrevContactState.push_back( leg->GetLoad() >= load_threshold );
			return false;
		}

		bool has_new_contact = false;
		for ( size_t idx = 0; idx < model.GetLegCount(); ++idx )
		{
			bool contact = model.GetLeg( idx ).GetLoad() >= load_threshold;
			has_new_contact |= contact && !m_PrevContactState[ idx ];
			m_PrevContactState[ idx ] = contact;
		}

		return has_new_contact;
	}

	void StepLengthMeasure::StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const
	{
		if ( !step_length.IsNull() )
			frame[ GetName() + ".step_length_penalty" ] = step_length.GetLatest();
	}
}

/*
** HeightMeasure.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "HeightMeasure.h"
#include "scone/model/Model.h"
#include "scone/core/Log.h"
#include "scone/core/profiler_config.h"

namespace scone
{
	HeightMeasure::HeightMeasure( const PropNode& props, Params& par, Model& model, const Location& loc ) :
		Measure( props, par, model, loc ),
		m_pTargetBody( nullptr ),
		m_JumpState( InitialState )
	{
		INIT_PROP( props, target_body, String( "" ) );
		INIT_PROP( props, use_average_height, false );
		INIT_PROP( props, terminate_on_peak, true );
		INIT_PROP( props, termination_height, 0.5 );
		INIT_PROP( props, max_admitted_counter_height, 0.0 );
		INIT_PROP( props, ignore_time, 0.05 );
		INIT_PROP( props, upward_velocity_threshold, 0.05 );
		INIT_PROP( props, downward_velocity_threshold, -0.05 );
		INIT_PROP( props, require_downward_movement, false );

		m_Upward = false;
		m_Height.Reset();

		// find target body
		if ( !target_body.empty() )
			m_pTargetBody = FindByName( model.GetBodies(), target_body ).get();
		else m_pTargetBody = nullptr;

		// init start height and initial height
		m_InitialHeight = m_pTargetBody ? m_pTargetBody->GetComPos()[ 1 ] : model.GetComPos()[ 1 ];
	}

	bool HeightMeasure::UpdateMeasure( const Model& model, double timestamp )
	{
		SCONE_PROFILE_FUNCTION;

		double pos = m_pTargetBody ? m_pTargetBody->GetComPos()[ 1 ] : model.GetComPos()[ 1 ];
		double vel = m_pTargetBody ? m_pTargetBody->GetComVel()[ 1 ] : model.GetComVel()[ 1 ];

		// add sample
		m_Height.AddSample( timestamp, pos );

		// check if the height is still high enough
		if ( pos < termination_height * m_Height.GetInitial() )
			return true;

		// update the state
		if ( timestamp > ignore_time )
		{
			switch ( m_JumpState )
			{
			case InitialState:
				if ( vel <= downward_velocity_threshold )
					m_JumpState = DownState;
				else if ( vel >= upward_velocity_threshold )
				{
					if ( require_downward_movement )
						return true; // moving up too soon
					else m_JumpState = UpState;
				}
				break;
			case DownState:
				if ( vel >= upward_velocity_threshold )
					m_JumpState = UpState;
				break;
			case UpState:
				if ( terminate_on_peak && vel < 0.0 )
					return true;
				break;
			}
		}

		return false;
	}

	double HeightMeasure::ComputeResult( Model& model )
	{
		if ( m_JumpState == InitialState )
			return 100 * ( termination_height - 1 ) * m_InitialHeight; // same score as just falling

		// compute admissible start height
		double lo_height = std::max( m_InitialHeight - max_admitted_counter_height, m_Height.GetLowest() );
		double hi_height = terminate_on_peak ? m_Height.GetLatest() : m_Height.GetHighest();

		GetReport().set( "Initial", m_InitialHeight );
		GetReport().set( "JumpStartHeight", m_Height.GetLowest() );
		GetReport().set( "lo_height", lo_height );
		GetReport().set( "hi_height", hi_height );

		// results are in cm to get nice scaling
		double result = use_average_height ? 100 * m_Height.GetAverage() : 100 * ( hi_height - lo_height );
		GetReport().set( result );
		return result;
	}

	scone::String HeightMeasure::GetClassSignature() const
	{
		return "Height";
	}
}

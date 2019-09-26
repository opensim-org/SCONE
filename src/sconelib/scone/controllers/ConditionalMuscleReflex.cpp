/*
** ConditionalMuscleReflex.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "ConditionalMuscleReflex.h"
#include "scone/model/Location.h"
#include "scone/model/Sensors.h"
#include "scone/model/Dof.h"
#include "scone/core/Range.h"

namespace scone
{
	ConditionalMuscleReflex::ConditionalMuscleReflex( const PropNode& props, Params& par, Model& model, const Location& loc ) :
	MuscleReflex( props, par, model, loc ),
	dof( *FindByLocation( model.GetDofs(), props.get< String >( "dof" ), loc ) ),
	m_pConditionalDofPos( nullptr ),
	m_pConditionalDofVel( nullptr )
	{
		m_pConditionalDofPos = &model.AcquireDelayedSensor< DofPositionSensor >( dof );
		m_pConditionalDofVel = &model.AcquireDelayedSensor< DofVelocitySensor >( dof );

		ScopedParamSetPrefixer prefixer( par, GetParName( props, loc ) + "-" + props.get< String >( "dof" ) + "." );
		INIT_PAR( props, par, pos_max, 1e12 );
		INIT_PAR( props, par, pos_min, -1e12 );
		m_ConditionalPosRange.max = pos_max;
		m_ConditionalPosRange.min = pos_min;
	}

	ConditionalMuscleReflex::~ConditionalMuscleReflex()
	{}

	void ConditionalMuscleReflex::ComputeControls( double timestamp )
	{
		// check the condition
		bool suppress = false;
		auto dofpos = m_pConditionalDofPos->GetValue( delay );
		if ( !m_ConditionalPosRange.Test( dofpos ) )
		{
			// check if the sign of the violation is equal to the sign of the velocity
			Real violation = m_ConditionalPosRange.GetRangeViolation( dofpos );
			Real dofvel = m_pConditionalDofVel->GetValue( delay );
			if ( std::signbit( violation ) == std::signbit( dofvel ) )
			{
				//log::Trace( m_Target.GetName( ) + ": Ignoring, " + VARSTR( violation ) + VARSTR( dofpos ) + VARSTR( dofvel ) );
				suppress = true;
			}
		}

		if ( suppress )
		{
			// only output constant, disable reflexes
			u_l = u_v = u_f = u_s = 0;
			u_total = C0;
			AddTargetControlValue( u_total );
		}
		else MuscleReflex::ComputeControls( timestamp );
	}
}

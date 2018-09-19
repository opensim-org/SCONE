#include "ConditionalMuscleReflex.h"
#include "scone/model/Location.h"
#include "scone/model/Sensors.h"
#include "scone/model/Dof.h"
#include "scone/core/Range.h"

namespace scone
{
	ConditionalMuscleReflex::ConditionalMuscleReflex( const PropNode& props, Params& par, Model& model, const Location& loc ) :
	MuscleReflex( props, par, model, loc ),
	m_pConditionalDofPos( nullptr ),
	m_pConditionalDofVel( nullptr ),
	dof( *FindByName( model.GetDofs(), loc.ConvertName( props.get< String >( "dof" ) ) ) )
	{
		m_pConditionalDofPos = &model.AcquireDelayedSensor< DofPositionSensor >( dof );
		m_pConditionalDofVel = &model.AcquireDelayedSensor< DofVelocitySensor >( dof );

		ScopedParamSetPrefixer prefixer( par, GetParName( props ) + "-" + props.get< String >( "dof" ) + "." );
		INIT_PAR( props, par, pos_max, 180 );
		INIT_PAR( props, par, pos_min, -180 );
		m_ConditionalPosRange.max = pos_max;
		m_ConditionalPosRange.min = pos_min;
	}

	ConditionalMuscleReflex::~ConditionalMuscleReflex()
	{}

	void ConditionalMuscleReflex::ComputeControls( double timestamp )
	{
		// check the condition
		bool suppress = false;
		Degree dofpos = Radian( m_pConditionalDofPos->GetValue( delay ) );
		if ( !m_ConditionalPosRange.Test( dofpos ) )
		{
			// check if the sign of the violation is equal to the sign of the velocity
			Real violation = m_ConditionalPosRange.GetRangeViolation( dofpos ).value;
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

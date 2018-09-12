#include "ConditionalMuscleReflex.h"
#include "scone/model/Locality.h"
#include "scone/model/Sensors.h"
#include "scone/model/Dof.h"
#include "scone/core/Range.h"

namespace scone
{
	ConditionalMuscleReflex::ConditionalMuscleReflex( const PropNode& props, Params& par, Model& model, const Locality& area ) :
	MuscleReflex( props, par, model, area ),
	m_pConditionalDofPos( nullptr ),
	m_pConditionalDofVel( nullptr ),
	dof( *FindByName( model.GetDofs(), area.ConvertName( props.get< String >( "dof" ) ) ) )
	{
		m_pConditionalDofPos = &model.AcquireDelayedSensor< DofPositionSensor >( dof );
		m_pConditionalDofVel = &model.AcquireDelayedSensor< DofVelocitySensor >( dof );

		ScopedParamSetPrefixer prefixer( par, GetParName( props ) + "-" + props.get< String >( "dof" ) + "." );

		m_ConditionalPosRange.max = Degree( par.try_get( "pos_max", props, "pos_max", 180.0 ) );
		m_ConditionalPosRange.min = Degree( par.try_get( "pos_min", props, "pos_min", -180.0 ) );

		//log::TraceF( "ConditionalMuscleReflex DOF=%s min=%.2f max=%.2f", dof.GetName().c_str(), m_ConditionalPosRange.min, m_ConditionalPosRange.max );
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
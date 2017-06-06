#include "ConditionalMuscleReflex.h"
#include "scone/model/Locality.h"
#include "scone/model/Sensors.h"
#include "scone/model/Dof.h"

namespace scone
{
	ConditionalMuscleReflex::ConditionalMuscleReflex( const PropNode& props, ParamSet& par, Model& model, const Locality& area ) :
	MuscleReflex( props, par, model, area ),
	m_pConditionalDofPos( nullptr ),
	m_pConditionalDofVel( nullptr )
	{
		const PropNode& cp = props.get_child( "Condition" );
		Dof& dof = *FindByName( model.GetDofs(), area.ConvertName( cp.get< String >( "dof" ) ) );
		m_pConditionalDofPos = &model.AcquireDelayedSensor< DofPositionSensor >( dof );
		m_pConditionalDofVel = &model.AcquireDelayedSensor< DofVelocitySensor >( dof );

		ScopedParamSetPrefixer prefixer( par, GetParName( props ) + "-" + cp.get< String >( "dof" ) + "." );

		if ( cp.has_key( "pos_range" ) )
		{
			// the 'old' non-parameterizable way (for backwards compatibility)
			m_ConditionalPosRange = Range< Degree >( cp.get_child( "pos_range" ) );
		}
		else
		{
			m_ConditionalPosRange.max = Degree( par.get_or( "pos_max", cp.try_get_child( "pos_max" ), 180.0 ) );
			m_ConditionalPosRange.min = Degree( par.get_or( "pos_min", cp.try_get_child( "pos_min" ), -180.0 ) );
		}

		//log::TraceF( "ConditionalMuscleReflex DOF=%s min=%.2f max=%.2f", dof.GetName().c_str(), m_ConditionalPosRange.min, m_ConditionalPosRange.max );
	}

	ConditionalMuscleReflex::~ConditionalMuscleReflex()
	{}

	void ConditionalMuscleReflex::ComputeControls( double timestamp )
	{
		// check the condition
		bool condition = true;
		Degree dofpos = Radian( m_pConditionalDofPos->GetValue( delay ) );
		if ( !m_ConditionalPosRange.Test( dofpos ) )
		{
			// check if the sign of the violation is equal to the sign of the velocity
			Real violation = m_ConditionalPosRange.GetRangeViolation( dofpos ).value;
			Real dofvel = m_pConditionalDofVel->GetValue( delay );
			if ( std::signbit( violation ) == std::signbit( dofvel ) )
			{
				//log::Trace( m_Target.GetName( ) + ": Ignoring, " + VARSTR( violation ) + VARSTR( dofpos ) + VARSTR( dofvel ) );
				condition = false;
			}
		}

		if ( condition )
			MuscleReflex::ComputeControls( timestamp );
	}
}
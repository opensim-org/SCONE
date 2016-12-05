#include "ConditionalMuscleReflex.h"
#include "scone/sim/Area.h"
#include "scone/sim/Sensors.h"
#include "scone/sim/Dof.h"

namespace scone
{
	namespace cs
	{
		ConditionalMuscleReflex::ConditionalMuscleReflex( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area ) :
		MuscleReflex( props, par, model, area ),
		m_pConditionalDofPos( nullptr ),
		m_pConditionalDofVel( nullptr )
		{
			const PropNode& cp = props.get_child( "Condition" );
			sim::Dof& dof = *FindByName( model.GetDofs(), cp.get< String >( "dof" ) + GetSideName( area.side ) );
			m_pConditionalDofPos = &model.AcquireDelayedSensor< sim::DofPositionSensor >( dof );
			m_pConditionalDofVel = &model.AcquireDelayedSensor< sim::DofVelocitySensor >( dof );
			m_ConditionalPosRange = Range< Degree >( cp.get_child( "pos_range" ) );
		}

		ConditionalMuscleReflex::~ConditionalMuscleReflex()
		{

		}

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
}
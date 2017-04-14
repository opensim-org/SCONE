#include "SensorStateController.h"

#include "scone/model/Model.h"
#include "flut/string_tools.hpp"

namespace scone
{
	
	SensorStateController::SensorStateController( const PropNode& props, ParamSet& par, Model& model, const Area& area ) :
	StateController( props, par, model, area )
	{
		// create states
		const PropNode& states_pn = props.get_child( "SensorStates" );
		for ( auto it = states_pn.begin(); it != states_pn.end(); ++it )
			m_States.push_back( SensorState( it->second, par ) );

		SCONE_ASSERT( m_States.size() >= 1 );

		// create conditional controllers
		CreateConditionalControllers( props, par, model, area );

		// init initial state
		UpdateCurrentState( model, 0.0 );
	}

	SensorStateController::SensorState::SensorState( const PropNode& pn, ParamSet& par )
	{
		INIT_PARAM_NAMED( pn, par, leg_load[ 0 ], "leg_load0", 0.0 );
		INIT_PARAM_NAMED( pn, par, leg_load[ 1 ], "leg_load1", 0.0 );
		INIT_PARAM_NAMED( pn, par, sag_pos[ 0 ], "sag_pos0", 0.0 );
		INIT_PARAM_NAMED( pn, par, sag_pos[ 1 ], "sag_pos1", 0.0 );
	}

	double SensorStateController::SensorState::GetDistance( Model& model, double timestamp ) const
	{
		double dist = 0;
		for ( Index idx = 0; idx < model.GetLegCount(); ++idx )
		{
			auto& leg = model.GetLeg( GetLegIndex( idx, mirrored ) );
			dist += flut::math::squared( leg.GetLoad() - leg_load[ idx ] );

			double sp = leg.GetFootLink().GetBody().GetComPos().x - leg.GetBaseLink().GetBody().GetComPos().x;
			dist += flut::math::squared( sp - sag_pos[ idx ] );
		}
		return dist;
	}

	scone::StateController::StateIndex SensorStateController::GetCurrentState( Model& model, double timestamp )
	{
		// find closest sensor state
		double best_dist = REAL_MAX;
		StateIndex best_idx = NoIndex;
		for ( StateIndex idx = 0; idx < m_States.size(); ++idx )
		{
			double d = m_States[ idx ].GetDistance( model, timestamp );
			if ( d < best_dist )
			{
				best_dist = d;
				best_idx = idx;
			}
		}
		return best_idx;
	}

	String SensorStateController::GetClassSignature() const
	{
		return flut::stringf( "SSC%d", GetStateCount() );
	}
}

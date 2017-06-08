#include "SensorStateController.h"

#include "scone/model/Model.h"
#include "flut/string_tools.hpp"

namespace scone
{
	SensorStateController::SensorStateController( const PropNode& props, Params& par, Model& model, const Locality& area ) :
	StateController( props, par, model, area )
	{
		// create states
		const PropNode& states_pn = props.get_child( "SensorStates" );
		for ( auto it = states_pn.begin(); it != states_pn.end(); ++it )
		{
			m_States.push_back( SensorState( it->second, par, area ) );
			m_States.push_back( SensorState( it->second, par, MakeMirrored( area ) ) );
		}

		SCONE_ASSERT( m_States.size() >= 1 );
		m_StateDist.resize( m_States.size() );

		// create conditional controllers
		CreateConditionalControllers( props, par, model, area );
		CreateConditionalControllers( props, par, model, MakeMirrored( area ) );

		// init initial state
		UpdateCurrentState( model, 0.0 );
	}

	void SensorStateController::StoreData( Storage<Real>::Frame& frame )
	{
		StateController::StoreData( frame );
		frame[ "ssc_current_state" ] = (double)m_CurrentState;
		for ( size_t idx = 0; idx < m_States.size(); ++idx )
		{
			frame[ "ssc_dist_" + flut::to_str( idx ) ] = m_StateDist[ idx ];
			frame[ "ssc_ld_" + flut::to_str( idx ) ] = m_States[ idx ].ld;
			frame[ "ssc_sd_" + flut::to_str( idx ) ] = m_States[ idx ].sd;
		}
	}

	SensorStateController::SensorState::SensorState( const PropNode& pn, Params& par, const Locality& a )
	{
		INIT_PROP_REQUIRED( pn, name );

		ScopedParamSetPrefixer pfx( par, name + "." );
		INIT_PARAM( pn, par, load_delta, 0.0 );
		INIT_PARAM( pn, par, sag_delta, 0.0 );

		if ( a.mirrored ) name += "_mirrored";
		mirrored = a.mirrored;

		//INIT_PARAM_NAMED( pn, par, leg_load[ 0 ], "leg_load0", 0.0 );
		//INIT_PARAM_NAMED( pn, par, leg_load[ 1 ], "leg_load1", 0.0 );
		//INIT_PARAM_NAMED( pn, par, sag_pos[ 0 ], "sag_pos0", 0.0 );
		//INIT_PARAM_NAMED( pn, par, sag_pos[ 1 ], "sag_pos1", 0.0 );
	}

	double SensorStateController::SensorState::GetDistance( Model& model, double timestamp )
	{
		auto& leg = model.GetLeg( GetLegIndex( 0, mirrored ) );
		auto& other_leg = model.GetLeg( GetLegIndex( 1, mirrored ) );

		ld = leg.GetLoad() - other_leg.GetLoad();
		sd = leg.GetRelFootPos().x - other_leg.GetRelFootPos().x;

		double dist = abs( load_delta - ld ) + abs( sag_delta - sd );
		return dist;
	}

	scone::StateController::StateIndex SensorStateController::GetCurrentState( Model& model, double timestamp )
	{
		// find closest sensor state
		double best_dist = REAL_MAX;
		StateIndex best_idx = NoIndex;
		for ( StateIndex idx = 0; idx < m_States.size(); ++idx )
			m_StateDist[ idx ] = m_States[ idx ].GetDistance( model, timestamp );

		auto min_it = std::min_element( m_StateDist.begin(), m_StateDist.end() );
		Index min_idx = min_it - m_StateDist.begin();
		return min_idx;
	}

	String SensorStateController::GetClassSignature() const
	{
		return flut::stringf( "SSC%d", GetStateCount() );
	}
}

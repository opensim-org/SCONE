#include "SensorStateController.h"

#include "scone/model/Model.h"
#include "xo/string/string_tools.h"
#include "scone/core/math.h"

namespace scone
{
	SensorStateController::SensorStateController( const PropNode& props, Params& par, Model& model, const Location& loc ) :
	StateController( props, par, model, loc )
	{
		INIT_PROP( props, create_mirrored_state, true );
		mirrored = loc.mirrored;

		// create states
		const PropNode& states_pn = props.get_child( "SensorStates" );
		for ( auto it = states_pn.begin(); it != states_pn.end(); ++it )
		{
			m_States.push_back( SensorState( it->second, par, loc ) );
			if ( create_mirrored_state )
				m_States.push_back( SensorState( it->second, par, MakeMirrored( loc ) ) );
		}

		SCONE_ASSERT( m_States.size() >= 1 );
		m_StateDist.resize( m_States.size() );

		// create conditional controllers
		CreateConditionalControllers( props, par, model, loc );
		if ( create_mirrored_state )
			CreateConditionalControllers( props, par, model, MakeMirrored( loc ) );

		// init initial state
		UpdateCurrentState( model, 0.0 );
	}

	void SensorStateController::StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const
	{
		StateController::StoreData( frame, flags );

		string prefix = create_mirrored_state ? "ssc" : "ssc" + xo::to_str( mirrored );
		frame[ prefix + "_state" ] = (double)m_CurrentState;
		for ( size_t idx = 0; idx < m_States.size(); ++idx )
		{
			string postfix = xo::to_str( idx );
			frame[ prefix + "_d" + postfix ] = m_StateDist[ idx ];
			frame[ prefix + "_ld" + postfix ] = m_States[ idx ].ld;
			frame[ prefix + "_sd" + postfix ] = m_States[ idx ].sd;
		}
	}

	SensorStateController::SensorState::SensorState( const PropNode& pn, Params& par, const Location& a )
	{
		INIT_PROP_REQUIRED( pn, name );

		ScopedParamSetPrefixer pfx( par, name + "." );
		INIT_PAR( pn, par, load_delta, 0.0 );
		INIT_PAR( pn, par, sag_delta, 0.0 );

		if ( a.mirrored ) name += "_mirrored";
		mirrored = a.mirrored;
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
		index_t min_idx = min_it - m_StateDist.begin();
		return min_idx;
	}

	String SensorStateController::GetClassSignature() const
	{
		return xo::stringf( "SSC%d", GetStateCount() );
	}
}

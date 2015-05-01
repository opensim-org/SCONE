#include "stdafx.h"
#include "FeedForwardController.h"

#include <boost/foreach.hpp>

#include "../core/InitFromPropNode.h"
#include "../core/Log.h"

#include "../sim/sim.h"
#include "../sim/Controller.h"
#include "../sim/Model.h"
#include "../sim/Muscle.h"

#include "Tools.h"
#include "Factories.h"
#include "../sim/Area.h"

namespace scone
{
	namespace cs
	{
		FeedForwardController::FeedForwardController( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& target_area ) :
		Controller( props, par, model, target_area )
		{
			DECLARE_AND_INIT( props, bool, use_symmetric_actuators, true );
			INIT_PROPERTY( props, number_of_modes, 0u );

			// setup actuator info
			for ( size_t idx = 0; idx < model.GetMuscleCount(); ++idx )
			{
				ActInfo ai;
				ai.full_name = model.GetMuscle( idx ).GetName();
				ai.name = GetNameNoSide( ai.full_name );
				ai.side = GetSide( ai.full_name );
				ai.muscle_idx = idx;

				// see if this muscle is on the right side
				if ( target_area.side == NoSide || target_area.side == ai.side )
					m_ActInfos.push_back( ai );
			}

			// create functions
			if ( UseModes() )
			{
				// create mode functions
				for ( size_t idx = 0; idx < number_of_modes; ++idx )
				{
					opt::ScopedParamSetPrefixer prefixer( par, GetStringF( "Mode%d.", idx ) );
					m_Functions.push_back( FunctionUP( scone::CreateFunction( props.GetChild( "Function" ), par ) ) );
				}
			}

			BOOST_FOREACH( ActInfo& ai, m_ActInfos )
			{
				if ( use_symmetric_actuators )
				{
					// check if we've already processed a mirrored version of this ActInfo
					auto it = std::find_if( m_ActInfos.begin(), m_ActInfos.end(), [&]( ActInfo& oai ) { return ai.name == oai.name; } );
					if ( it->function_idx != NoIndex || !it->mode_weights.empty() )
					{
						ai.function_idx = it->function_idx;
						ai.mode_weights = it->mode_weights;
						continue;
					}
				}

				if ( UseModes() )
				{
					// set mode weights
					ai.mode_weights.resize( number_of_modes );
					String prefix = use_symmetric_actuators ? ai.name : ai.full_name;
					for ( size_t mode = 0; mode < number_of_modes; ++mode )
						ai.mode_weights[ mode ] = par.Get( prefix + GetStringF( ".Mode%d", mode ), props.GetChild( "mode_weight" ) );
				}
				else
				{
					// create a new function
					String prefix = use_symmetric_actuators ? ai.name : ai.full_name;
					opt::ScopedParamSetPrefixer prefixer( par, prefix + "." );
					m_Functions.push_back( FunctionUP( scone::CreateFunction( props.GetChild( "Function" ), par ) ) );
					ai.function_idx = m_Functions.size() - 1;
				}
			}
		}

		void FeedForwardController::UpdateControls( sim::Model& model, double time )
		{
			// evaluate functions
			std::vector< double > funcresults( m_Functions.size() );
			for ( size_t idx = 0; idx < m_Functions.size(); ++idx )
				funcresults[ idx ] = m_Functions[ idx ]->GetValue( time );

			// apply results to all actuators
			BOOST_FOREACH( ActInfo& ai, m_ActInfos )
			{
				if ( UseModes() )
				{
					Real val = 0.0;
					for ( size_t mode = 0; mode < number_of_modes; ++mode )
						val += ai.mode_weights[ mode ] * funcresults[ mode ];

					// add control value
					model.GetMuscle( ai.muscle_idx ).AddControlValue( val );
				}
				else
				{
					// apply results directly to control value
					model.GetMuscle( ai.muscle_idx ).AddControlValue( funcresults[ ai.function_idx ] );
				}
			}
		}

		scone::String FeedForwardController::GetSignature()
		{
			String s = "F" + m_Functions.front()->GetSignature();
			if ( number_of_modes > 0 )
				s += GetStringF( "M%d", number_of_modes );

			return s;
		}
	}
}

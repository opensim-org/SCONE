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

namespace scone
{
	namespace cs
	{
		FeedForwardController::FeedForwardController( const PropNode& props, opt::ParamSet& par, sim::Model& model ) :
		Controller( props, par, model )
		{
			INIT_FROM_PROP( props, use_symmetric_actuators, true );
			INIT_FROM_PROP( props, number_of_modes, 0u );

			// setup actuator info
			for ( size_t idx = 0; idx < model.GetMuscleCount(); ++idx )
			{
				ActInfo ai;
				ai.full_name = model.GetMuscle( idx ).GetName();
				ExtractNameAndSide( model.GetMuscle( idx ).GetName(), ai.name, ai.side );
				m_ActInfos.push_back( ai );
			}

			// create functions
			if ( UseModes() )
			{
				// create mode functions
				for ( size_t idx = 0; idx < number_of_modes; ++idx )
				{
					par.PushNamePrefix( GetStringF( "Mode%d.", idx ) );
					m_Functions.push_back( FunctionUP( scone::CreateFunction( props.GetChild( "Function" ), par ) ) );
					par.PopNamePrefix();
				}
			}

			BOOST_FOREACH( ActInfo& ai, m_ActInfos )
			{
				if ( use_symmetric_actuators )
				{
					// check if we've already processed a mirrored version of this ActInfo
					auto it = std::find_if( m_ActInfos.begin(), m_ActInfos.end(), [&]( ActInfo& oai ) { return ai.name == oai.name; } );
					if ( it->function_idx != NO_INDEX || !it->mode_weights.empty() )
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
					par.PushNamePrefix( prefix + "." );
					m_Functions.push_back( FunctionUP( scone::CreateFunction( props.GetChild( "Function" ), par ) ) );
					par.PopNamePrefix();
					ai.function_idx = m_Functions.size() - 1;
				}
			}
		}

		void FeedForwardController::UpdateControls( sim::Model& model, double time )
		{
			// update controls for both sides
			UpdateControls( model, time, NoSide );
		}

		void FeedForwardController::UpdateControls( sim::Model& model, double time, Side side )
		{
			// evaluate functions
			std::vector< double > funcresults( m_Functions.size() );
			for ( size_t idx = 0; idx < m_Functions.size(); ++idx )
				funcresults[ idx ] = m_Functions[ idx ]->GetValue( time );

			// apply result of each mode to all muscles
			for ( size_t idx = 0; idx < m_ActInfos.size(); ++idx )
			{
				if ( side == NoSide || m_ActInfos[ idx ].side == side )
				{
					if ( UseModes() )
					{
						Real val = 0.0;
						for ( size_t mode = 0; mode < number_of_modes; ++mode )
							val += funcresults[ mode ] * m_ActInfos[ idx ].mode_weights[ mode ];

						// add control value
						model.GetMuscle( idx ).AddControlValue( val );

					}
					else
					{
						// apply results directly to control value
						model.GetMuscle( idx ).AddControlValue( funcresults[ m_ActInfos[ idx ].function_idx ] );
					}
				}
			}
		}
	}
}

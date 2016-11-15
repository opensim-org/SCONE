#include "StateController.h"

#include "scone/sim/Factories.h"

namespace scone
{
	namespace cs
	{
		StateController::StateController( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area ) :
		sim::Controller( props, par, model, area )
		{
		}

		StateController::~StateController()
		{
		}

		void StateController::CreateConditionalControllers( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area )
		{
			// create instances for each controller
			const PropNode& ccProps = props.get_child( "ConditionalControllers" );
			for ( PropNode::const_iterator ccIt = ccProps.begin(); ccIt != ccProps.end(); ++ccIt )
			{
				// get state masks
				String state_masks = ccIt->second->GetStr( "states" );
				auto state_tokens = flut::split_str( state_masks, ";," );
				for ( const String& instance_states : state_tokens )
				{
					// create new conditional controller
					m_ConditionalControllers.push_back( ConditionalController() );
					ConditionalController& cc = m_ConditionalControllers.back();
					ConditionalControllerState& ccs = cc.first;
					ccs.is_active = false;
					ccs.is_active_since = 0;

					// initialize state_mask based on names in instance_states (TODO: use tokenizer?)
					String bit_string( GetStateCount(), '0' );
					ccs.state_mask.resize( GetStateCount(), false );
					bool has_any_state = false;
					for ( size_t i = 0; i < GetStateCount(); ++i )
					{
						if ( instance_states.find( GetStateName( i ) ) != String::npos )
						{
							ccs.state_mask[ i ] = has_any_state = true;
							bit_string[ GetStateCount() - 1 - i ] = '1';
						}
					}
					SCONE_THROW_IF( !has_any_state, "Conditional Controller has empty state mask" )

					// create controller
					const PropNode& cprops = ccIt->second->get_child( "Controller" );
					opt::ScopedParamSetPrefixer prefixer( par, "S" + bit_string + "." );
					cc.second = sim::CreateController( cprops, par, model, area );
				}
			}
		}

		void StateController::UpdateConditionalControllerStates( StateIndex current_state, TimeInSeconds timestamp )
		{
			SCONE_ASSERT( current_state < GetStateCount() );

			// update controller states
			for ( ConditionalController& cc: m_ConditionalControllers )
			{
				ConditionalControllerState& ccs = cc.first;
				bool activate = ccs.state_mask[ current_state ];

				// activate or deactivate controller
				if ( activate != ccs.is_active )
				{
					ccs.is_active = activate; 
					ccs.is_active_since = timestamp;
				}
			}
		}

		StateController::UpdateResult StateController::UpdateControls( sim::Model& model, double timestamp )
		{
			for ( ConditionalController& cc: m_ConditionalControllers )
			{
				if ( cc.first.is_active )
					cc.second->UpdateControls( model, timestamp - cc.first.is_active_since );
			}

			return SuccessfulUpdate;
		}

		scone::String StateController::GetClassSignature() const 
		{
			String s = "S";

			std::map< String, int > controllers;
			for ( const ConditionalController& cc: m_ConditionalControllers )
				controllers[ cc.second->GetSignature() ] += 1;

			// output number of controllers per leg
			for ( auto it = controllers.begin(); it != controllers.end(); ++it )
				s += "_" + to_str( it->second ) + it->first;

			return s;
		}
	}
}

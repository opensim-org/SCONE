#include "StateController.h"

#include "scone/core/Factories.h"
#include "scone/core/string_tools.h"
#include "scone/core/Log.h"

namespace scone
{
	StateController::StateController( const PropNode& props, Params& par, Model& model, const Location& loc ) :
	Controller( props, par, model, loc ),
	m_CurrentState( NoIndex )
	{
	}

	void StateController::CreateConditionalControllers( const PropNode& props, Params& par, Model& model, const Location& loc )
	{
		// create instances for each controller
		const PropNode& ccProps = props.get_child( "ConditionalControllers" );
		for ( PropNode::const_iterator ccIt = ccProps.begin(); ccIt != ccProps.end(); ++ccIt )
		{
			// get state masks
			String state_masks = ccIt->second.get< String >( "states" );
			auto state_tokens = xo::split_str( state_masks, ";," );
			for ( String controller_state_name : state_tokens )
			{
				// fix name for mirrored case -- yes, this is pretty nasty
				SCONE_THROW_NOT_IMPLEMENTED;
				// TODO: implement this
				//if ( loc.mirrored )
				//	controller_state_name += "_mirrored";

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
					if ( controller_state_name == GetStateName( i ) )
					{
						// This may not work for the original SSC examples
						ccs.state_mask[ i ] = has_any_state = true;
						size_t bit_string_idx = GetStateCount() - 1 - i;
						bit_string[ bit_string_idx ] = '1';
					}
				}
				SCONE_THROW_IF( !has_any_state, "Conditional Controller has empty state mask" );

				// create controller
				const PropNode& cprops = ccIt->second.get_child( "Controller" );
				ScopedParamSetPrefixer prefixer( par, "S" + bit_string + "." );
				cc.second = CreateController( cprops, par, model, loc );
			}
		}
	}

	void StateController::UpdateCurrentState( Model& model, TimeInSeconds timestamp )
	{
		auto current_state = GetCurrentState( model, timestamp );
		if ( current_state != m_CurrentState )
		{
			SCONE_ASSERT( current_state < GetStateCount() );

			m_CurrentState = current_state;

			// update controller states
			for ( ConditionalController& cc : m_ConditionalControllers )
			{
				ConditionalControllerState& ccs = cc.first;
				bool activate = ccs.state_mask[ m_CurrentState ];

				// activate or deactivate controller
				if ( activate != ccs.is_active )
				{
					ccs.is_active = activate;
					ccs.is_active_since = timestamp;
				}
			}

			//for ( int i = 0; i < m_ConditionalControllers.size(); ++i )
			//	log::info( "Controller ", i, " active=", m_ConditionalControllers[ i ].first.is_active );
		}
	}

	bool StateController::ComputeControls( Model& model, double timestamp )
	{
		// adjust current state if needed
		UpdateCurrentState( model, timestamp );

		// process active controllers
		for ( ConditionalController& cc : m_ConditionalControllers )
		{
			if ( cc.first.is_active )
				cc.second->UpdateControls( model, timestamp - cc.first.is_active_since );
		}

		return false;
	}

	void StateController::StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const
	{
		for ( auto& cc : m_ConditionalControllers )
		{
			if ( cc.first.is_active )
				cc.second->StoreData( frame, flags );
		}
	}

	scone::String StateController::GetClassSignature() const
	{
		String s = "S";

		std::map< String, int > controllers;
		for ( const ConditionalController& cc : m_ConditionalControllers )
			controllers[ cc.second->GetSignature() ] += 1;

		// output number of controllers per leg
		for ( auto it = controllers.begin(); it != controllers.end(); ++it )
			s += "_" + to_str( it->second ) + it->first;

		return s;
	}
}

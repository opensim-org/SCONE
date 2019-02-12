/*
** FeedForwardController.cpp
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "FeedForwardController.h"

#include "scone/controllers/Controller.h"
#include "scone/model/Muscle.h"
#include "scone/model/Location.h"

#include "scone/core/Factories.h"
#include "scone/core/Profiler.h"

namespace scone
{
	FeedForwardController::FeedForwardController( const PropNode& props, Params& par, Model& model, const Location& target_area ) :
	Controller( props, par, model, target_area ),
	Function( props.get_child( "Function" ) )
	{
		INIT_PROP( props, symmetric, true );

		// setup actuator info
		for ( size_t idx = 0; idx < model.GetMuscles().size(); ++idx )
		{
			ActInfo ai;
			ai.full_name = model.GetMuscles()[ idx ]->GetName();
			ai.name = GetNameNoSide( ai.full_name );
			ai.side = GetSideFromName( ai.full_name );
			ai.muscle_idx = idx;

			// see if this muscle is on the right side
			if ( target_area.side == NoSide || target_area.side == ai.side )
				m_ActInfos.push_back( ai );
		}

		for ( ActInfo& ai : m_ActInfos )
		{
			if ( symmetric )
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

			// create a new function
			String prefix = symmetric ? ai.name : ai.full_name;
			ScopedParamSetPrefixer prefixer( par, prefix + "." );
			auto fp = FindFactoryProps( GetFunctionFactory(), props, "Function" );
			m_Functions.push_back( CreateFunction( fp, par ) );
			ai.function_idx = m_Functions.size() - 1;
		}
	}

	bool FeedForwardController::ComputeControls( Model& model, double time )
	{
		SCONE_PROFILE_FUNCTION;

		// evaluate functions
		std::vector< double > funcresults( m_Functions.size() );
		for ( size_t idx = 0; idx < m_Functions.size(); ++idx )
			funcresults[ idx ] = m_Functions[ idx ]->GetValue( time );

		// apply results to all actuators
		for ( ActInfo& ai : m_ActInfos )
		{
			// apply results directly to control value
			model.GetMuscles()[ ai.muscle_idx ]->AddInput( funcresults[ ai.function_idx ] );
		}

		return false;
	}

	scone::String FeedForwardController::GetClassSignature() const
	{
		String s = "F" + m_Functions.front()->GetSignature();

		return s;
	}
}

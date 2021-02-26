/*
** FeedForwardController.cpp
**
** Copyright (C) 2013-2021 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "FeedForwardController.h"

#include "scone/controllers/Controller.h"
#include "scone/model/Muscle.h"
#include "scone/model/Location.h"

#include "scone/core/Factories.h"
#include "scone/core/profiler_config.h"

namespace scone
{
	FeedForwardController::FeedForwardController( const PropNode& props, Params& par, Model& model, const Location& target_area ) :
		Controller( props, par, model, target_area ),
		INIT_MEMBER( props, symmetric, target_area.symmetric_ ),
		INIT_MEMBER( props, include, "*" ),
		INIT_MEMBER( props, exclude, "" )
	{
		INIT_PROP( props, symmetric, target_area.symmetric_ );

		// setup actuator info
		auto incl = xo::pattern_matcher( include );
		auto excl = xo::pattern_matcher( exclude );
		auto& actuators = model.GetActuators();
		for ( size_t idx = 0; idx < actuators.size(); ++idx )
		{
			const auto& name = actuators[ idx ]->GetName();
			if ( incl( name ) && !excl( name ) )
			{
				ActInfo ai;
				ai.full_name = actuators[ idx ]->GetName();
				ai.name = GetNameNoSide( ai.full_name );
				ai.side = GetSideFromName( ai.full_name );
				ai.actuator_idx = idx;

				// see if this actuator is on the right side
				if ( target_area.side_ == NoSide || target_area.side_ == ai.side )
					m_ActInfos.push_back( ai );
			}
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
		SCONE_PROFILE_FUNCTION( model.GetProfiler() );

		// evaluate functions
		std::vector< double > funcresults( m_Functions.size() );
		for ( size_t idx = 0; idx < m_Functions.size(); ++idx )
			funcresults[ idx ] = m_Functions[ idx ]->GetValue( time );

		// apply results to all actuators
		auto& actuators = model.GetActuators();
		for ( ActInfo& ai : m_ActInfos )
		{
			// apply results directly to control value
			actuators[ ai.actuator_idx ]->AddInput( funcresults[ ai.function_idx ] );
		}

		return false;
	}

	scone::String FeedForwardController::GetClassSignature() const
	{
		if ( !m_Functions.empty() )
			return "F" + m_Functions.front()->GetSignature();
		else return String();
	}
}

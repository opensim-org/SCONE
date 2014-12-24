#include "stdafx.h"

#include "SimulationObjective.h"
#include "../core/Exception.h"

#include <boost/foreach.hpp>
#include <algorithm>

namespace scone
{
	namespace cs
	{
		SimulationObjective::SimulationObjective()
		{
		}

		SimulationObjective::~SimulationObjective()
		{
		}

		double SimulationObjective::Evaluate()
		{
			sim::SimulationUP s = CreateFromPropNode< sim::Simulation >( m_Props );
			s->ProcessParameters( m_Params );

			// find measure controller
			auto& controllers = s->GetModel( 0 ).GetControllers();
			auto measureIter = std::find_if( controllers.begin(), controllers.end(), [&]( sim::ControllerUP& c ){ return dynamic_cast< Measure* >( c.get() ) != nullptr; } );

			if ( measureIter == controllers.end() )
				SCONE_THROW( "Could not find a measure" );
			else if ( controllers.end() != std::find_if( measureIter, controllers.end(), [&]( sim::ControllerUP& c ){ return dynamic_cast< Measure* >( c.get() ) != nullptr; } ) )
				SCONE_THROW( "More than one measure was found" );

			Measure& m = dynamic_cast< Measure& >( **measureIter );

			// run the simulation
			s->AdvanceSimulationTo( max_duration );

			return m.GetValue();
		}

		void SimulationObjective::ProcessProperties( const PropNode& props )
		{
			INIT_FROM_PROP( props, max_duration, 6000.0 );

			// just copy properties
			m_Props = props;
		}

		void SimulationObjective::ProcessParameters( opt::ParamSet& par )
		{
			// just copy parameters
			m_Params = par;
		}

	}
}

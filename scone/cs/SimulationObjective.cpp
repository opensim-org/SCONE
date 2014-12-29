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
			// find measure controller
			auto& controllers = m_Model->GetControllers();
			auto measureIter = std::find_if( controllers.begin(), controllers.end(), [&]( sim::ControllerUP& c ){ return dynamic_cast< Measure* >( c.get() ) != nullptr; } );

			if ( measureIter == controllers.end() )
				SCONE_THROW( "Could not find a measure" );
			else if ( controllers.end() != std::find_if( measureIter + 1, controllers.end(), [&]( sim::ControllerUP& c ){ return dynamic_cast< Measure* >( c.get() ) != nullptr; } ) )
				SCONE_THROW( "More than one measure was found" );

			Measure& m = dynamic_cast< Measure& >( **measureIter );

			// run the simulation
			m_Model->AdvanceSimulationTo( max_duration );

			return m.GetResult( *m_Model );
		}

		void SimulationObjective::ProcessProperties( const PropNode& props )
		{
			INIT_FROM_PROP( props, max_duration, 6000.0 );

			m_Model = CreateFromPropNode< sim::Model >( props.GetChild( "Model" ) );
		}

		void SimulationObjective::ProcessParameters( opt::ParamSet& par )
		{
			m_Model->ProcessParameters( par );
		}

		void SimulationObjective::WriteResults( const String& file )
		{
			m_Model->WriteStateHistory( file );
		}
	}
}

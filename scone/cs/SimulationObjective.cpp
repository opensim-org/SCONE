#include "stdafx.h"

#include "SimulationObjective.h"
#include "../core/Exception.h"

#include <boost/foreach.hpp>
#include <algorithm>

#include "FactoryTest.h"
#include "../sim/Model.h"
#include "../sim/Factories.h"

namespace scone
{
	namespace cs
	{
		SimulationObjective::SimulationObjective( const PropNode& props ) :
		Objective( props )
		{
			INIT_FROM_PROP( props, max_duration, 6000.0 );
			//InitFromPropNodeChild( props, m_Model, "Model" );
			//InitFromPropNode( props.GetChild( "Model" ), m_Model );
			//ModelFactory f = GetModelFactories()[ props.GetChild( "Model" ).GetStr( "type" ) ];
			const PropNode& p = props.GetChild( "Model" );

			sim::CreateModelFunc f = sim::GetModelFactory().GetCreateFunc( "Simbody" );
			//sim::Model* m = f( p );
			m_Model = sim::ModelUP( f( p ) );
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

		void SimulationObjective::ProcessParameters( opt::ParamSet& par )
		{
			// process model parameters (model should reset)
			m_Model->ProcessParameters( par );
		}

		void SimulationObjective::WriteResults( const String& file )
		{
			m_Model->WriteStateHistory( file );
		}
	}
}

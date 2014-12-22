#include "stdafx.h"

#include "Simulation_Simbody.h"
#include "../../core/Exception.h"
#include "Model_Simbody.h"
#include "../../core/Propertyable.h"

namespace scone
{
	namespace sim
	{
		scone::sim::ModelUP Simulation_Simbody::CreateModel( const String& filename )
		{
			return ModelUP( new Model_Simbody( *this, filename ) );
		}

		void Simulation_Simbody::AdvanceSimulationTo( double time )
		{
			for ( auto iter = m_Models.begin(); iter != m_Models.end(); ++iter )
				dynamic_cast< Model_Simbody& >( **iter ).AdvanceSimulationTo( time );
		}

		void Simulation_Simbody::ProcessProperties( const PropNode& props )
		{
			Simulation::ProcessProperties( props );

			INIT_FROM_PROP( props, integration_accuracy, 0.00001 );
		}
	}
}

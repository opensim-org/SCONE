#include "Simulation_Simbody.h"
#include "scone/core/Exception.h"
#include "Model_Simbody.h"
#include "scone/core/propnode_tools.h"

namespace scone
{
	namespace sim
	{
		Simulation_Simbody::Simulation_Simbody( const PropNode& props ) :
		Simulation( props )
		{
		}

		void Simulation_Simbody::AdvanceSimulationTo( double time )
		{
			for ( auto iter = m_Models.begin(); iter != m_Models.end(); ++iter )
				dynamic_cast< Model_Simbody& >( **iter ).AdvanceSimulationTo( time );
		}
	}
}

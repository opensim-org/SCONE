#include "Simulation.h"

#include "scone/core/InitFromPropNode.h"

namespace scone
{
	namespace sim
	{
		Simulation::Simulation( const PropNode& props ) :
		max_simulation_time( 0.0 ),
		integration_accuracy( 0.0 )
		{
			INIT_PROPERTY( props, integration_accuracy, 0.00001 );
			INIT_PROPERTY( props, max_simulation_time, 10000.0 );
		}

		Simulation::~Simulation()
		{

		}

		Model& Simulation::GetModel( size_t idx )
		{
			SCONE_ASSERT( idx < m_Models.size() );
			return *m_Models[ idx ];
		}
	}
}

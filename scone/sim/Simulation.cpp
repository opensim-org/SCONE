#include "stdafx.h"
#include "Simulation.h"

#include <boost/foreach.hpp>
#include "../core/InitFromPropNode.h"

namespace scone
{
	namespace sim
	{
		Simulation::Simulation( const PropNode& props ) :
		max_simulation_time( 0.0 ),
		integration_accuracy( 0.0 )
		{
			INIT_FROM_PROP( props, integration_accuracy, 0.00001 );
			INIT_FROM_PROP( props, max_simulation_time, 10000.0 );

			//TOFIX: InitFromPropNode( props, m_Models );
		}

		Simulation::~Simulation()
		{

		}

		Model& Simulation::GetModel( size_t idx )
		{
			SCONE_ASSERT( idx < m_Models.size() );
			return *m_Models[ idx ];
		}

		void Simulation::ProcessParameters( opt::ParamSet& par )
		{
			BOOST_FOREACH( ModelUP& model, m_Models )
				model->ProcessParameters( par );
		}
	}
}

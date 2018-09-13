#include "SimulationSimbody.h"
#include "scone/core/Exception.h"
#include "ModelSimbody.h"

namespace scone
{
	SimulationSimbody::SimulationSimbody( const PropNode& props ) :
		Simulation( props )
	{
	}

	void SimulationSimbody::AdvanceSimulationTo( double time )
	{
		for ( auto iter = m_Models.begin(); iter != m_Models.end(); ++iter )
			dynamic_cast<ModelSimbody&>( **iter ).AdvanceSimulationTo( time );
	}
}

#include "stdafx.h"
#include "Simulation_Simbody.h"
#include "../../core/Exception.h"
#include "Model_Simbody.h"

namespace scone
{
	namespace sim
	{
		scone::sim::ModelUP Simulation_Simbody::CreateModel( const String& filename )
		{
			return ModelUP( new Model_Simbody( filename ) );
		}

		void Simulation_Simbody::AdvanceSimulationTo( double time )
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}

		void Simulation_Simbody::ProcessProperties( const PropNode& props )
		{

		}
	}
}

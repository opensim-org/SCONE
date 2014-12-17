#include "stdafx.h"
#include "Simulation_Simbody.h"
#include "..\..\core\Exception.h"
#include "Model_Simbody.h"

namespace scone
{
	namespace sim
	{
		ModelUP Simulation_Simbody::CreateModel()
		{
			return ModelUP( new Model_Simbody() );
		}

		void Simulation_Simbody::Run()
		{
			throw std::logic_error("The method or operation is not implemented.");
		}

		void Simulation_Simbody::ProcessProperties( const PropNode& props )
		{

		}
	}
}

#include "stdafx.h"
#include "Simulation_Simbody.h"
#include "..\..\core\Exception.h"
#include "Model_Simbody.h"

namespace scone
{
	namespace sim
	{
		ModelSP Simulation_Simbody::CreateModel()
		{
			return std::make_shared< Model_Simbody >( this );
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

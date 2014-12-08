#include "stdafx.h"
#include "Simulation_Simbody.h"
#include "..\..\core\Exception.h"

namespace scone
{
	namespace sim
	{
		ModelSP Simulation_Simbody::CreateModel( const String& filename )
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}

		ModelSP Simulation_Simbody::GetModel( size_t idx )
		{
			throw std::logic_error("The method or operation is not implemented.");
		}

		void Simulation_Simbody::Run()
		{
			throw std::logic_error("The method or operation is not implemented.");
		}
	}
}

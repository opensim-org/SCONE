#pragma once

#include "scone/core/core.h"

#ifdef SCONE_SIM_SIMBODY_EXPORTS
#define SCONE_SIM_SIMBODY_API __declspec(dllexport)
#else
#define SCONE_SIM_SIMBODY_API __declspec(dllimport)
#endif

namespace scone
{
	namespace sim
	{
		SCONE_DECLARE_CLASS_AND_PTR( Simulation_Simbody );
		void SCONE_SIM_SIMBODY_API RegisterSimbody();
	}
}

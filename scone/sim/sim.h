#pragma once

#include <memory>
#include "../core/core.h"

#ifdef SCONE_SIM_EXPORTS
#define SCONE_SIM_API __declspec(dllexport)
#else
#define SCONE_SIM_API __declspec(dllimport)
#endif

namespace scone
{
	namespace sim
	{
		// forward declarations
		SCONE_DECLARE_CLASS_AND_PTR( Simulation );
		SCONE_DECLARE_CLASS_AND_PTR( Model );
		SCONE_DECLARE_CLASS_AND_PTR( Body );
		SCONE_DECLARE_CLASS_AND_PTR( Joint );
		SCONE_DECLARE_CLASS_AND_PTR( Link );
		SCONE_DECLARE_CLASS_AND_PTR( Actuator );
		SCONE_DECLARE_CLASS_AND_PTR( Muscle );
		SCONE_DECLARE_CLASS_AND_PTR( Controller );
		SCONE_DECLARE_CLASS_AND_PTR( Leg );
	}
}

#pragma once

#include "scone/core/core.h"
#include "scone/core/memory_tools.h"

namespace scone
{
	namespace sim
	{
		// forward declarations
		SCONE_DECLARE_CLASS_AND_PTR( Simulation );
		SCONE_DECLARE_CLASS_AND_PTR( Model );
		SCONE_DECLARE_CLASS_AND_PTR( Body );
		SCONE_DECLARE_CLASS_AND_PTR( Joint );
		SCONE_DECLARE_CLASS_AND_PTR( Dof );
		SCONE_DECLARE_CLASS_AND_PTR( Link );
		SCONE_DECLARE_CLASS_AND_PTR( Area );
		SCONE_DECLARE_CLASS_AND_PTR( Actuator );
		SCONE_DECLARE_CLASS_AND_PTR( Muscle );
		SCONE_DECLARE_CLASS_AND_PTR( Controller );
		SCONE_DECLARE_CLASS_AND_PTR( Leg );
		SCONE_DECLARE_CLASS_AND_PTR( Sensor );
		SCONE_DECLARE_CLASS_AND_PTR( SensorDelayAdapter );

		void SCONE_API RegisterFactoryTypes();
	}
}

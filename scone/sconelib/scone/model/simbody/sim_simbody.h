#pragma once

#include "scone/core/core.h"
#include "scone/core/memory_tools.h"

namespace scone
{
	namespace sim
	{
		SCONE_DECLARE_CLASS_AND_PTR( Simulation_Simbody );
		void SCONE_API RegisterSimbody();
	}
}

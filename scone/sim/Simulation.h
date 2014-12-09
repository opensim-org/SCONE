#pragma once

#include "sim.h"
#include "Model.h"
#include <memory>
#include "..\core\PropNode.h"

namespace scone
{
	namespace sim
	{
		class SIM_API Simulation : public Propertyable
		{
		public:
			Simulation();
			virtual ~Simulation();
			virtual ModelSP CreateModel( const String& filename ) = 0;
			virtual void ProcessProperties(const PropNode& props);

			virtual ModelSP GetModel( size_t idx = 0 ) = 0;
			virtual void Run() = 0;
		};
	}
}

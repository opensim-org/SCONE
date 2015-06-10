#pragma once

#include "sim.h"
#include "../core/PropNode.h"
#include "../opt/ParamSet.h"

namespace scone
{
	namespace sim
	{
		class SCONE_SIM_API Sensor
		{
		public:
			Sensor( const PropNode& pn, opt::ParamSet& par, sim::Model& model, const Area& target_area );
			virtual ~Sensor();

			virtual Real GetValue() const = 0;
		};
	}
}

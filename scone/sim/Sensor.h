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
			Sensor();
			Sensor( const PropNode& pn, opt::ParamSet& par, sim::Model& model, const Area& target_area );
			virtual ~Sensor();

			virtual Real GetValue() const = 0;
			virtual String GetName() const = 0;

			// special case for multichannel sensors
			virtual Count GetChannelCount() { return 1; }
			virtual Real* GetValueArray() const { SCONE_THROW_NOT_IMPLEMENTED; }
		};
	}
}

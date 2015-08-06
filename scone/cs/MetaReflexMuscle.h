#pragma once

#include "cs.h"
#include "../core/core.h"
#include "../sim/Muscle.h"
#include <vector>

namespace scone
{
	namespace cs
	{
		class CS_API MetaReflexMuscle
		{
		public:
			MetaReflexMuscle( sim::Muscle& mus, sim::Model& model, const std::vector< MetaReflexDofUP >& dofs );
			virtual ~MetaReflexMuscle();
			void UpdateControls();

			sim::Muscle& muscle;
			sim::SensorDelayAdapter& force_sensor;
			sim::SensorDelayAdapter& length_sensor;

			// info
			Real ref_length;
			Real delay;
			size_t dof_count;

			// gain parameters
			Real length_gain;
			Real constant_ex;
			Real force_gain;
			Real stiffness;
		};
	}
}

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
			MetaReflexMuscle( sim::Muscle& mus, sim::Model& model, const MetaReflexController& controller, const sim::Area& area );
			virtual ~MetaReflexMuscle();
			void UpdateControls();
			void InitMuscleParameters( const MetaReflexController& controller );
			sim::Muscle& muscle;
			sim::SensorDelayAdapter& force_sensor;
			sim::SensorDelayAdapter& length_sensor;

			// info
			Real ref_length;
			Real delay;
			Real total_abs_moment_arm;

			// gain parameters
			Real length_gain;
			Real constant_ex;
			Real force_gain;
			Real stiffness;

			// dof specific info
			struct DofInfo
			{
				MetaReflexDof* dof;
				Real moment_arm;
				Real max_moment;
				Real contrib_weight;
			};
			std::vector< DofInfo > dof_infos;

		private:
			Real ComputeStiffnessExcitation( MetaReflexDof& dof );
		};
	}
}

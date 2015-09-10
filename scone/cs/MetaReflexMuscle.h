#pragma once

#include "cs.h"

#include "../core/core.h"
#include "../sim/Muscle.h"
#include <vector>
#include "MetaReflexParams.h"

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
			void UpdateMuscleControlParameters();
			sim::Muscle& muscle;
			sim::SensorDelayAdapter& force_sensor;
			sim::SensorDelayAdapter& length_sensor;

			Real delay;
			Real total_abs_moment_arm;

			// info
			Real base_length;

			// muscle control parameters
			Real ref_length;
			Real length_gain;
			Real constant;
			Real force_gain;
			Real stiffness;

			// dof specific info
			struct DofInfo
			{
				DofInfo( MetaReflexDof& d ) : dof( d ) {}
				MetaReflexDof& dof;
				Real w;
				Real abs_w;
				Real moment_arm;
				Real max_moment;
				Real lengthening_speed;
			};
			std::vector< DofInfo > dof_infos;
		private:
			Real ComputeStiffnessExcitation( MetaReflexDof& dof );
		};
	}
}

#pragma once

#include "../core/PropNode.h"
#include "../opt/ParamSet.h"
#include "../sim/Model.h"
#include "../sim/Area.h"
#include "../sim/SensorDelayAdapter.h"
#include "../sim/Dof.h"
#include "MetaReflexParams.h"
#include <vector>

namespace scone
{
	namespace cs
	{
		class MetaReflexVirtualMuscle 
		{
		public:
			MetaReflexVirtualMuscle( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area );
			virtual ~MetaReflexVirtualMuscle() {}

			Real GetLocalBalance() { return 0.0; } // TODO!

			String name;

			MetaReflexParams mrp;
			MetaReflexParams bal_mrp;
			Real delay;

			Real GetSimilarity( const sim::Muscle& mus, Real tot_abs_moment_arm );

		private:

			struct DofInfo {
				sim::Dof& dof;
				Real w;
			};
			std::vector< DofInfo > dof_infos;

			Real local_balance;
			Real body_angvel_sensor_gain;
			sim::SensorDelayAdapter* body_ori_sensor;
			sim::SensorDelayAdapter* body_angvel_sensor;
		};
	}
}

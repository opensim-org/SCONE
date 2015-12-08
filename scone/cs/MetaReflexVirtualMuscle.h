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

			Real GetLocalBalance() { return local_balance; }
			void UpdateLocalBalance( const Vec3& global_balance );

			String name;

			MetaReflexParams mrp;
			MetaReflexParams bal_mrp;
			Real delay;

			Real GetSimilarity( const sim::Muscle& mus, Real tot_abs_moment_arm );
			size_t GetDofCount() { return dof_infos.size(); }
			void SetDofRotationAxis();
			Vec3 average_moment_axis;

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
			Real body_sensor_delay;
		};
	}
}

#pragma once

#include "scone/core/PropNode.h"
#include "scone/optimization/Params.h"
#include "scone/model/Model.h"
#include "scone/model/Locality.h"
#include "scone/model/SensorDelayAdapter.h"
#include "scone/model/Dof.h"
#include "MetaReflexParams.h"
#include <vector>

namespace scone
{
	class MetaReflexVirtualMuscle
	{
	public:
		MetaReflexVirtualMuscle( const PropNode& props, Params& par, Model& model, const Locality& area );
		virtual ~MetaReflexVirtualMuscle() {}

		Real GetLocalBalance() { return local_balance; }
		void UpdateLocalBalance( const Vec3& global_balance );

		String name;

		MetaReflexParams mrp;
		MetaReflexParams bal_mrp;
		Real delay;

		Real GetSimilarity( const Muscle& mus, Real tot_abs_moment_arm );
		size_t GetDofCount() { return dof_infos.size(); }
		Vec3 average_moment_axis;

	private:

		struct DofInfo {
			Dof& dof;
			Real w;
		};
		std::vector< DofInfo > dof_infos;

		Real local_balance;
		Real body_angvel_sensor_gain;
		SensorDelayAdapter* body_ori_sensor;
		SensorDelayAdapter* body_angvel_sensor;
		Real body_sensor_delay;
	};
}

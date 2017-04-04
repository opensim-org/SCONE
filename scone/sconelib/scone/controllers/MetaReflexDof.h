#pragma once

#include "scone/core/PropNode.h"
#include "scone/model/Model.h"
#include "scone/optimization/ParamSet.h"
#include "MetaReflexParams.h"
#include "scone/model/SensorDelayAdapter.h"
#include "scone/core/HasData.h"
#include "scone/core/Vec3.h"

namespace scone
{
	namespace cs
	{
		// TODO: derive from either Reflex or Controller, or don't and remove this TODO
		class SCONE_API MetaReflexDof : public HasData
		{
		public:
			MetaReflexDof( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area );
			virtual ~MetaReflexDof() {}

			void SetupUsingCurrentPose();
			void UpdateControls();
			void AddAvailableMoment( Real max_moment );

			void UpdateLocalBalance( const Vec3& global_balance );

			sim::Dof& target_dof;
			typedef int TargetDir;

			MetaReflexParams dof_pos;
			MetaReflexParams bal_pos;
			MetaReflexParams dof_neg;
			MetaReflexParams bal_neg;

			// delay, TODO: move this to muscle
			Real delay;

			// max muscle moments
			Real tot_available_neg_mom;
			Real tot_available_pos_mom;

			// dof axis in world coordinates at target pose
			void SetDofRotationAxis();
			Vec3 dof_rotation_axis;

			Real GetLocalBalance();
			virtual void StoreData( Storage< Real >::Frame& frame ) override;

		private:
			bool MuscleCrossesDof( const sim::Muscle& mus );

			Real local_balance;
			Real body_angvel_sensor_gain;
			sim::SensorDelayAdapter* body_ori_sensor;
			sim::SensorDelayAdapter* body_angvel_sensor;
			Real body_sensor_delay;
		};
	}
}

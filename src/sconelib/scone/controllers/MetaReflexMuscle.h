#pragma once

#include "scone/core/core.h"
#include "scone/core/types.h"
#include "scone/model/Muscle.h"
#include <vector>
#include "MetaReflexParams.h"
#include "MetaReflexVirtualMuscle.h"

namespace scone
{
	class SCONE_API MetaReflexMuscle
	{
	public:
		MetaReflexMuscle( Muscle& mus, Model& model, const MetaReflexController& controller, const Locality& area );
		virtual ~MetaReflexMuscle();
		void UpdateControls();
		void UpdateMuscleControlParameters( bool debug = false );
		Muscle& muscle;
		SensorDelayAdapter& force_sensor;
		SensorDelayAdapter& length_sensor;

		Real delay;
		Real total_abs_moment_arm;

		// info
		Real ref_length_base;

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

		struct VirtualMuscleInfo
		{
			MetaReflexVirtualMuscle* vm;
			Real similarity;
		};
		Real total_vm_similarity;

		std::vector< DofInfo > dof_infos;
		std::vector< VirtualMuscleInfo > vm_infos;

	private:
		Real ComputeStiffnessExcitation( MetaReflexDof& dof );
	};
}

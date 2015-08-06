#pragma once

#include "cs.h"
#include "../core/PropNode.h"
#include "../sim/Model.h"
#include "../opt/ParamSet.h"

namespace scone
{
	namespace cs
	{
		// TODO: derive from either Reflex or Controller
		class CS_API MetaReflexDof
		{
		public:
			MetaReflexDof( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area );
			virtual ~MetaReflexDof();

			void SetupUsingCurrentPose();
			void UpdateControls();

			friend struct MuscleInfo;

			sim::Dof& target_dof;
			Degree ref_pos_in_deg;
			Real ref_pos_in_rad;
			Real length_gain;
			Real constant;
			Real force_feedback;
			Real delay; // TODO: move this to muscle

		private:
			bool MuscleCrossesDof( const sim::Muscle& mus );
		};
	}
}

#pragma once

#include "cs.h"
#include "../core/PropNode.h"
#include "../sim/Model.h"
#include "../opt/ParamSet.h"
#include "MetaReflexParams.h"

namespace scone
{
	namespace cs
	{
		// TODO: derive from either Reflex or Controller, or don't and remove this TODO
		class CS_API MetaReflexDof
		{
		public:
			MetaReflexDof( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area );
			virtual ~MetaReflexDof();

			void SetupUsingCurrentPose();
			void UpdateControls();
			void AddAvailableMoment( Real max_moment );

			sim::Dof& target_dof;

			MetaReflexParams dof_par;
			MetaReflexParams bal_par;

			// delay, TODO: move this to muscle
			Real delay;

			// max muscle moments
			Real tot_available_neg_mom;
			Real tot_available_pos_mom;

		private:
			bool MuscleCrossesDof( const sim::Muscle& mus );
		};
	}
}

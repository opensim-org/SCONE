#pragma once

#include "cs.h"
#include "../core/PropNode.h"
#include "../sim/Model.h"
#include "../opt/ParamSet.h"

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

			// actual parameters
			struct DofParams
			{
				void InitFromPropNode( const PropNode& props, opt::ParamSet& par, sim::Model& model );

				Degree ref_pos_in_deg;
				Real length_gain;
				Real constant;
				Real force_feedback;
				Real stiffness;
			};
			DofParams dof_par;

			// regulation parameters
			sim::SensorDelayAdapter* reg_sensor;
			DofParams reg_par;

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

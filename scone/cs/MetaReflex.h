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
		class CS_API MetaReflex
		{
		public:
			MetaReflex( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area );
			virtual ~MetaReflex();

			void SetupUsingCurrentPose();
			void UpdateControls();

			friend struct MuscleInfo;

			sim::Dof& target_dof;
			Real reference_pos_in_degrees;
			Real reference_pos_in_radians;
			Real length_gain;
			Real constant;
			Real force_feedback;
			Real delay; // TODO: move this to muscle

		private:
			// Structure taking
			struct MuscleInfo
			{
				MuscleInfo( sim::Model& model, sim::Muscle& mus );
				void UpdateControls( MetaReflex& mr );

				sim::Muscle* muscle;
				Real moment_arm;
				Real reference_length;

				sim::SensorDelayAdapter* force_sensor;
				sim::SensorDelayAdapter* length_sensor;
			};

			std::vector< MuscleInfo > m_MuscleInfos;
			bool MuscleCrossesDof( const sim::Muscle& mus );
		};
	}
}

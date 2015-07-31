#pragma once

#include "cs.h"
#include "../core/PropNode.h"
#include "../sim/Model.h"
#include "../opt/ParamSet.h"

namespace scone
{
	namespace cs
	{
		class CS_API MetaReflex
		{
		public:
			MetaReflex( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area );
			virtual ~MetaReflex();

			void SetupUsingCurrentPose();
			void ComputeControls();

			friend struct MuscleInfo;

		private:
			sim::Dof& target_dof;
			Real reference_pos;
			Real length_gain;
			Real constant_moment;
			Real force_feedback;
			Real delay; // TODO: move this to muscle

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

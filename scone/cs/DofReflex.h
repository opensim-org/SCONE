#pragma once

#include "Reflex.h"

namespace scone
{
	namespace cs
	{
		class CS_API DofReflex : public Reflex
		{
		public:
			DofReflex( const PropNode& props, opt::ParamSet& par, sim::Model& model, sim::Actuator& target, sim::Sensor& source );
			virtual ~DofReflex();

			virtual void ComputeControls( double timestamp );

			// Reflex parameters
			Real target_pos;
			Real target_vel;
			Real pos_gain;
			Real vel_gain;
		};
	}
}

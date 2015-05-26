#pragma once

#include "cs.h"
#include "Reflex.h"

namespace scone
{
	namespace cs
	{
		class CS_API MuscleReflex : public Reflex
		{
		public:
			MuscleReflex( const PropNode& props, opt::ParamSet& par, sim::Model& model, sim::Actuator& target, sim::Sensor& source );
			virtual ~MuscleReflex();

			virtual void ComputeControls( double timestamp );

			// Reflex parameters
			Real length_gain;
			Real length_ofs;
			Real force_gain;
			Real velocity_gain;
		};
	}
}

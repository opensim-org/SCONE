#pragma once

#include "cs.h"
#include "../sim/Muscle.h"
#include "../core/PropNode.h"
#include "../core/Delayer.h"
#include "../opt/ParamSet.h"
#include "../sim/Model.h"

namespace scone
{
	namespace cs
	{
		class CS_API Reflex
		{
		public:
			Reflex( const PropNode& props, opt::ParamSet& par, sim::Model& model, sim::Muscle& source, sim::Muscle& target );
			virtual ~Reflex();

			virtual void ComputeControls( double timestamp );

			// Reflex parameters
			Real length_gain;
			Real length_ofs;
			Real force_gain;
			Real velocity_gain;

			TimeInSeconds delay;

		private:
			sim::Muscle& m_Source;
			sim::Muscle& m_Target;
			sim::SensorDelayAdapter& m_DelayedSource;
		};
	}
}

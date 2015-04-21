#pragma once

#include "cs.h"
#include "../sim/Muscle.h"
#include "../core/PropNode.h"
#include "../core/Delayed.h"
#include "../opt/ParamSet.h"

namespace scone
{
	namespace cs
	{
		class CS_API Reflex
		{
		public:
			Reflex( const PropNode& props, opt::ParamSet& par, sim::Muscle& source, sim::Muscle& target );
			virtual ~Reflex();

			virtual void UpdateControls( double timestamp );

			// Reflex parameters
			Real length_gain;
			Real length_ofs;
			Real force_gain;
			Real velocity_gain;

		private:
			sim::Muscle& m_Source;
			sim::Muscle& m_Target;

			// Delayed muscle parameters
			DelayedReal m_Length;
			DelayedReal m_Force;
			DelayedReal m_Velocity;
		};
	}
}

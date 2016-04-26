#pragma once

#include "Reflex.h"

namespace scone
{
	namespace cs
	{
		class SCONE_API DofReflex : public Reflex
		{
		public:
			DofReflex( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area );
			virtual ~DofReflex();

			virtual void ComputeControls( double timestamp );

			// Reflex parameters
			Real target_pos;
			Real target_vel;
			Real pos_gain;
			Real vel_gain;

		private:
			sim::SensorDelayAdapter& m_DelayedPos;
			sim::SensorDelayAdapter& m_DelayedVel;
			sim::SensorDelayAdapter& m_DelayedRootPos; // used for world coordinates, TODO: neater
			sim::SensorDelayAdapter& m_DelayedRootVel; // used for world coordinates, TODO: neater
			bool m_bUseRoot;
		};
	}
}

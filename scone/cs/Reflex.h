#pragma once

#include "cs.h"
#include "../sim/Model.h"
#include "../opt/ParamSet.h"
#include "../core/PropNode.h"

namespace scone
{
	namespace cs
	{
		class CS_API Reflex
		{
		public:
			Reflex( const PropNode& props, opt::ParamSet& par, sim::Model& model );
			Reflex( const PropNode& props, opt::ParamSet& par, sim::Model& model, sim::Sensor& source, Index source_idx, sim::Actuator& target );
			virtual ~Reflex();

			virtual void ComputeControls( double timestamp );

			Real gain;
			Real offset;
			TimeInSeconds delay;

		private:
			sim::SensorDelayAdapter& m_DelayedSource;
			Index m_SourceIdx;
			sim::Actuator& m_Target;
		};
	}
}

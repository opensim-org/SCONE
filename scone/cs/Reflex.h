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
			//Reflex( const PropNode& props, opt::ParamSet& par, sim::Model& model );
			Reflex( const PropNode& props, opt::ParamSet& par, sim::Model& model, sim::Actuator& target, sim::ChannelSensor& source );
			virtual ~Reflex();

			virtual void ComputeControls( double timestamp );
			TimeInSeconds delay;

		protected:
			Real GetDelayedSensorValue( Index channel ) { return m_DelayedSource.GetSensorValue( channel, delay ); }
			sim::SensorDelayAdapter& m_DelayedSource;
			sim::Actuator& m_Target;
		};
	}
}

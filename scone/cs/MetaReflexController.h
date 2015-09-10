#pragma once

#include "../sim/Controller.h"
#include "cs.h"

namespace scone
{
	namespace cs
	{
		class CS_API MetaReflexController : public sim::Controller
		{
		public:
			MetaReflexController( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area );
			virtual ~MetaReflexController();

			virtual UpdateResult UpdateControls( sim::Model& model, double timestamp ) override;

			const std::vector< MetaReflexDofUP >& GetReflexDofs() const { return m_ReflexDofs; }

		private:
			MetaReflexController( const MetaReflexController& other );
			MetaReflexController& operator=( const MetaReflexController& other );

			virtual String GetClassSignature() const override;

			std::vector< MetaReflexDofUP > m_ReflexDofs;
			std::vector< MetaReflexMuscleUP > m_ReflexMuscles;

			//sim::SensorDelayAdapter& m_SagBalSensor;
			//sim::SensorDelayAdapter& m_CorBalSensor;
			//sim::SensorDelayAdapter& m_TraBalSensor;
		};
	}
}

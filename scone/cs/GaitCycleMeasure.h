#pragma once

#include "cs.h"
#include "Measure.h"
#include "../sim/State.h"

namespace scone
{
	namespace cs
	{
		class CS_API GaitCycleMeasure : public Measure
		{
		public:
			GaitCycleMeasure( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area );
			virtual ~GaitCycleMeasure();

			virtual double GetResult( sim::Model& model ) override;

			bool use_half_cycle;

		private:
			Real GetStateSimilarity( sim::State& state );
			std::pair< String, int > GetMirroredStateNameAndSign( const String& str );

			sim::State m_InitState;
		};
	}
}

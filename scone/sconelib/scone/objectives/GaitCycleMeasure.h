#pragma once

#include "Measure.h"
#include "scone/model/State.h"

namespace scone
{
	namespace cs
	{
		class SCONE_API GaitCycleMeasure : public Measure
		{
		public:
			GaitCycleMeasure( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area );
			virtual ~GaitCycleMeasure();

			virtual double GetResult( sim::Model& model ) override;

			bool use_half_cycle;

		private:
			Real GetStateSimilarity( const State& state );
			std::pair< String, int > GetMirroredStateNameAndSign( const String& str );

			State m_InitState;
		};
	}
}

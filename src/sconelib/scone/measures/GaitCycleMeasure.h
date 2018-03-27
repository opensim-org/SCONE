#pragma once

#include "Measure.h"
#include "scone/model/State.h"

namespace scone
{
	class SCONE_API GaitCycleMeasure : public Measure
	{
	public:
		GaitCycleMeasure( const PropNode& props, Params& par, Model& model, const Locality& area );
		virtual ~GaitCycleMeasure();

		virtual double GetResult( Model& model ) override;

		bool use_half_cycle;

	protected:
		virtual UpdateResult UpdateMeasure( const Model& model, double timestamp ) override { return Controller::NoUpdate; }

	private:
		Real GetStateSimilarity( const State& state );
		std::pair< String, int > GetMirroredStateNameAndSign( const String& str );

		State m_InitState;

	};
}

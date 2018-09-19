#pragma once

#include "Measure.h"
#include "scone/model/State.h"

namespace scone
{
	/// Measure how well a single gait cycle is performed.
	/// For gait optimization, it is recommended to use GaitMeasure instead
	class GaitCycleMeasure : public Measure
	{
	public:
		GaitCycleMeasure( const PropNode& props, Params& par, Model& model, const Location& loc );
		virtual ~GaitCycleMeasure();

		virtual double GetResult( Model& model ) override;

		/// Use half gait cycle instead of full cycle; default = false.
		bool use_half_cycle;

	protected:
		virtual bool UpdateMeasure( const Model& model, double timestamp ) override { return false; }

	private:
		Real GetStateSimilarity( const State& state );
		std::pair< String, int > GetMirroredStateNameAndSign( const String& str );

		State m_InitState;

	};
}

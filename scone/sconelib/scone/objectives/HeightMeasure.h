#pragma once

#include "Measure.h"
#include "scone/model/Body.h"
#include "scone/core/Statistic.h"

namespace scone
{
	class HeightMeasure : public Measure
	{
	public:
		HeightMeasure( const PropNode& props, Params& par, Model& model, const Locality& area );
		virtual ~HeightMeasure() { };

		virtual UpdateResult UpdateAnalysis( const Model& model, double timestamp ) override;
		virtual double GetResult( Model& model ) override;

	protected:
		virtual String GetClassSignature() const override;

	private:
		enum { InitialState, DownState, UpState } m_JumpState;
		String target_body;
		bool use_average_height;
		bool terminate_on_peak;
		double termination_height;
		double max_admitted_counter_height;
		double ignore_time;
		double upward_velocity_threshold;
		double downward_velocity_threshold;

		Body* m_pTargetBody; // non-owning pointer
		Statistic< double > m_Height;
		bool m_Upward;
		double m_InitialHeight;
		bool require_downward_movement;
	};
}

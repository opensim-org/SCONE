#pragma once

#include "Measure.h"
#include "scone/model/Body.h"
#include "scone/core/Statistic.h"

namespace scone
{
	/// Measure for optimizing height, such as in jumping tasks.
	class HeightMeasure : public Measure
	{
	public:
		HeightMeasure( const PropNode& props, Params& par, Model& model, const Location& loc );
		virtual ~HeightMeasure() { };

		/// Body for which to measure the height, leave empty to use COM (default).
		String target_body;

		/// Use average height instead of peak hight for measure; default = false.
		bool use_average_height;

		/// Terminate simulation when a peak is detected; default = true.
		bool terminate_on_peak;

		/// Relative height of COM at which to terminate the simulation; default = 0.5.
		double termination_height;
		double max_admitted_counter_height;
		double ignore_time;
		double upward_velocity_threshold;
		double downward_velocity_threshold;

		virtual bool UpdateMeasure( const Model& model, double timestamp ) override;
		virtual double GetResult( Model& model ) override;

	protected:
		virtual String GetClassSignature() const override;

	private:
		enum { InitialState, DownState, UpState } m_JumpState;
		Body* m_pTargetBody; // non-owning pointer
		Statistic< double > m_Height;
		bool m_Upward;
		double m_InitialHeight;
		bool require_downward_movement;
	};
}

#pragma once
#include "Measure.h"
#include "scone/core/Statistic.h"
#include "EffortMeasure.h"
#include "DofLimitMeasure.h"

namespace scone
{
	// Class: GaitMeasure
	// Measure for efficient locomotion at a predefined speed
	class GaitMeasure : public Measure
	{
	public:
		GaitMeasure( const PropNode& props, Params& par, Model& model, const Locality& area );
		virtual ~GaitMeasure();

		// prop: termination_height
		// Relative COM height (wrt initial position) at which to stop the simulation
		Real termination_height = 0.5;

		// prop: min_velocity
		// Minimum velocity (m/s). Default = 0 m/s.
		Real min_velocity;

		// prop: max_velocity
		// Maximum velocity (m/s). Default = 299792458 m/s (speed of light)
		Real max_velocity = 299792458.0;

		// prop: load_threshold
		// Load threshold for step detection
		Real load_threshold = 0.1;

		// prop: min_step_duration
		// Minimum duration (in seconds) of a step, used for step detection. Default = 0.1
		Real min_step_duration = 0.1;

		// prop: initiation_steps
		// Number of initial steps of which the velocity is disregarded in the final measure. Default = 2.
		int initiation_steps = 2;

		virtual bool UpdateMeasure( const Model& model, double timestamp ) override;
		void AddStep( const Model &model, double timestamp );
		virtual double GetResult( Model& model ) override;
		virtual void StoreData( Storage<Real>::Frame& frame, const StoreDataFlags& flags ) const override;

	protected:
		virtual String GetClassSignature() const override;

	private:
		struct Step {
			TimeInSeconds time;
			Real length;
		};
		std::vector< Step > steps_;

		// settings
		String upper_body;
		String base_bodies;
		Body* m_UpperBody;
		std::vector< Body* > m_BaseBodies;
		Real GetGaitDist( const Model &model );

		bool HasNewFootContact( const Model& model );
		std::vector< bool > m_PrevContactState;

		Vec3 m_InitialComPos;
		Real m_InitGaitDist;
		Real m_PrevGaitDist;

		PropNode m_Report;
	};
}

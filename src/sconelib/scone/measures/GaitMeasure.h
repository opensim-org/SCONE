#pragma once
#include "Measure.h"
#include "scone/core/Statistic.h"
#include "EffortMeasure.h"
#include "DofLimitMeasure.h"

namespace scone
{
	class GaitMeasure : public Measure
	{
	public:
		GaitMeasure( const PropNode& props, Params& par, Model& model, const Locality& area );
		virtual ~GaitMeasure();

		/// Relative COM height (wrt initial position) at which to stop the simulation
		Real termination_height = 0.5;

		/// Minimum velocity (m/s)
		Real min_velocity = 0.5;

		/// Maximum velocity (m/s)
		Real max_velocity = 299792458.0;

		/// Load threshold for step detection
		Real load_threshold = 0.1;

		/// Minimum duration of a step, for step detection
		Real min_step_duration = 0.1;

		/// Number of initial steps of which the velocity is disregarded in the final measure
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

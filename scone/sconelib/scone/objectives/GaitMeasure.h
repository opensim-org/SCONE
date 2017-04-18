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
		GaitMeasure( const PropNode& props, ParamSet& par, Model& model, const Area& area );
		virtual ~GaitMeasure();

		virtual UpdateResult UpdateAnalysis( const Model& model, double timestamp ) override;
		void UpdateVelocityMeasure( const Model &model, double timestamp );

		virtual double GetResult( Model& model ) override;

		// parameters
		Real termination_height;
		Real min_max_velocity_weight;
		Real min_velocity;
		Real max_velocity;
		Real load_threshold;
		Real max_velocity_range;
		Real max_velocity_range_weight;

	protected:
		virtual String GetClassSignature() const override;

	private:
		Statistic< double > m_Energy;

		// settings
		std::vector< Body* > m_GaitBodies;
		Real GetGaitDist( const Model &model );

		bool HasNewFootContact( const Model& model );

		std::vector< bool > m_PrevContactState;

		Vec3 m_InitialComPos;
		Real m_InitGaitDist;
		Real m_PrevGaitDist;

		PropNode m_Report;

		Statistic<> m_MinMaxVelocityMeasure;
		Statistic<> m_VelocityRangeMeasure;
		int m_nSteps;
		double m_TotStepSize;
	};
}

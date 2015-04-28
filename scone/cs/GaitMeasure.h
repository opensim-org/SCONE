#pragma once
#include "Measure.h"
#include "../core/Statistic.h"
#include "EffortMeasure.h"
#include "DofLimitMeasure.h"

namespace scone
{
	namespace cs
	{
		class GaitMeasure : public Measure
		{
		public:
			GaitMeasure( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area );
			virtual ~GaitMeasure();

			virtual void UpdateControls( sim::Model& model, double timestamp ) override;
			virtual double GetResult( sim::Model& model ) override;

			virtual String GetSignature() override;

			// parameters
			Real termination_height;
			Real min_velocity;
			Real contact_force_threshold;

			struct WeightedTerm {
				WeightedTerm( double w = 0.0 ) : weight( w ), value( 0.0 ) {};
				double weighted_value() { return weight * value; }
				double weight;
				double value;
			};
			typedef std::pair< const String, WeightedTerm > StringWeightedTermPair;
			std::map< String, WeightedTerm > m_Terms;

		private:
			Statistic< double > m_Energy;
			double GetModelEnergy( sim::Model& model );

			// settings
			std::vector< sim::Body* > m_GaitBodies;
			Real GetGaitDist( sim::Model &model );

			bool HasNewFootContact( sim::Model& model );
			std::vector< bool > m_PrevContactState;

			Vec3 m_InitialComPos;
			Real m_InitGaitDist;
			Real m_PrevGaitDist;

			MeasuredReal m_MinVelocityMeasure;
			EffortMeasure m_EffortMeasure;
			DofLimitMeasure m_DofLimitMeasure;
		};
	}
}

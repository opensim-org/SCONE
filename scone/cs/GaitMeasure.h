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

			virtual UpdateResult UpdateAnalysis( const sim::Model& model, double timestamp ) override;
			void UpdateMinVelocityMeasure( const sim::Model &model, double timestamp );

			virtual double GetResult( sim::Model& model ) override;

			virtual PropNode GetReport();

			// parameters
			Real termination_height;
			Real min_velocity;
			Real contact_force_threshold;

		protected:
			virtual String GetMainSignature() const override;

		private:
			Statistic< double > m_Energy;

			// settings
			std::vector< sim::Body* > m_GaitBodies;
			Real GetGaitDist( const sim::Model &model );

			bool HasNewFootContact( const sim::Model& model );

			std::vector< bool > m_PrevContactState;

			Vec3 m_InitialComPos;
			Real m_InitGaitDist;
			Real m_PrevGaitDist;

			PropNode m_Report;

			Statistic<> m_MinVelocityMeasure;
		};
	}
}

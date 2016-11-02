#pragma once
#include "Measure.h"
#include "scone/core/StringMap.h"
#include "scone/core/Statistic.h"
#include "scone/core/Vec3.h"

namespace scone
{
	namespace cs
	{
		class EffortMeasure : public Measure
		{
		public:
			EffortMeasure( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area );
			virtual ~EffortMeasure();

			enum EnergyMeasureType { UnknownMeasure, TotalForce, Wang2012, Constant };
			static StringMap< EnergyMeasureType > m_MeasureNames;

			virtual UpdateResult UpdateAnalysis( const sim::Model& model, double timestamp ) override;
			virtual double GetResult( sim::Model& model ) override;

			EnergyMeasureType measure_type;
			bool use_cost_of_transport;

		protected:
			virtual String GetClassSignature() const override;

		private:
			Real m_Wang2012BasalEnergy;
			Statistic< double > m_Energy;
			Vec3 m_InitComPos;
			PropNode m_Report;
            Real specific_tension;
            Real muscle_density;
            bool use_Uchida2016_fiber_ratios;

			double GetEnergy( const sim::Model& model ) const;
			double GetWang2012( const sim::Model& model ) const;
			double GetTotalForce( const sim::Model& model ) const;
            void SetSlowTwitchRatios( sim::Model& model );
		};
	}
}

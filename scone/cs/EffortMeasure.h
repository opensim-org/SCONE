#pragma once
#include "Measure.h"
#include "../core/EnumStringMap.h"
#include "../core/Statistic.h"

namespace scone
{
	namespace cs
	{
		class EffortMeasure : public Measure
		{
		public:
			EffortMeasure( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area );
			virtual ~EffortMeasure();

			enum EnergyMeasureType { UnknownMeasure, TotalForce, Wang2012 };
			static EnumStringMap< EnergyMeasureType > m_MeasureNames;

			virtual void UpdateControls( sim::Model& model, double timestamp ) override;
			virtual double GetResult( sim::Model& model ) override;

			EnergyMeasureType measure_type;

		private:
			Real m_Wang2012BasalEnergy;
			Statistic< double > m_Energy;

			double GetEnergy( sim::Model& model );
			double GetWang2012( sim::Model& model );
			double GetTotalForce( sim::Model& model );
			virtual String GetSignature() override;
		};
	}
}

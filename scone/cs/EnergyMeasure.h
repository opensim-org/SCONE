#pragma once
#include "Measure.h"
//#include "../core/EnumStringMap.h"
#include "../core/SampledValue.h"

namespace scone
{
	namespace cs
	{
		class EnergyMeasure : public Measure
		{
		public:
			EnergyMeasure( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area );
			virtual ~EnergyMeasure();

			enum EnergyMeasureType { UnknownMeasure, TotalForce, Umberger2010 };
			//static EnumStringMap< EnergyMeasureType > m_MeasureNames;

			virtual void UpdateControls( sim::Model& model, double timestamp ) override;
			virtual double GetResult( sim::Model& model ) override;

		private:
			SampledValue< double > m_Energy;

			double GetTotalForce( sim::Model& model );
		};
	}
}

#pragma once

#include "cs.h"
#include "Measure.h"

#include "..\sim\sim.h"
#include "..\opt\Objective.h"
#include "..\opt\ParamSet.h"
#include "..\sim\Simulation.h"

namespace scone
{
	namespace cs
	{
		class CS_API Objective : public opt::Objective
		{
		public:
			Objective();
			virtual ~Objective();

			virtual opt::ParamSet GetParamSet() override;
			virtual double Evaluate( const opt::ParamSet& params ) override;

		private:
			std::vector< MeasureUP > m_Measures;

		private:
			// make class non copyable by declaring copy-ctor and assignment private
			Objective( const Objective& );
			Objective& operator=( const Objective& );
		};
	}
}

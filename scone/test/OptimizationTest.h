#pragma once

#include "..\cs\Optimization.h"
#include "..\opt\Objective.h"

namespace scone
{
	class ExampleObjective : public opt::Objective
	{
		virtual opt::ParamSet GetParamSet() override
		{
			throw std::logic_error("The method or operation is not implemented.");
		}

		virtual double Evaluate( const opt::ParamSet& params ) override
		{
			throw std::logic_error("The method or operation is not implemented.");
		}
	};
}

void OptimizationTest()
{
	cs::Optimization opt;
	opt.Run("config/optimization_test.xml");
};

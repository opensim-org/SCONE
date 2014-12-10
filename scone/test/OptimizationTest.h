#pragma once

#include "..\opt\Objective.h"
#include "..\opt\CmaOptimizer.h"
#include "..\opt\Optimization.h"
#include "..\opt\ParamSet.h"
#include "..\core\math.h"

namespace scone
{
	class ExampleObjective : public opt::Objective
	{
	public:
		SCONE_GENERATE_FACTORY_MEMBERS( ExampleObjective );

		ExampleObjective() : num_params( 0 ) { };
		virtual double Evaluate() override;
		virtual void ProcessProperties( const PropNode& props ) override;
		virtual void ProcessParameters( opt::ParamSet& par ) override;

	private:
		int num_params;
		std::vector< double > params;
	};

	void OptimizationTest();
}


#pragma once

#include "scone/optimization/Objective.h"

namespace scone
{
	class ExampleObjective : public Objective
	{
	public:
		ExampleObjective( const PropNode& props, ParamSet& par );
		virtual void ProcessParameters( ParamSet& par ) override;
		static double Rosenbrock( const std::vector< double >& v );

	protected:
		virtual double Evaluate() override;

	private:
		int num_params;
		std::vector< double > params;
		bool is_evaluating;
	};
}

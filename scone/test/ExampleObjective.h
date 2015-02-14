#pragma once

#include "../opt/Objective.h"
#include "../core/PropNodeFactory.h"

namespace scone
{
	class ExampleObjective : public opt::Objective, public Factoryable< opt::Objective, ExampleObjective >
	{
	public:
		ExampleObjective( const PropNode& props );;
		virtual void ProcessParameters( opt::ParamSet& par ) override;
		static double Rosenbrock( const std::vector< double >& v );

	protected:
		virtual double Evaluate() override;

	private:
		int num_params;
		std::vector< double > params;
		bool is_evaluating;
	};
}

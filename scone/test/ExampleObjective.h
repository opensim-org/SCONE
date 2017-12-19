#pragma once

#include "scone/optimization/Objective.h"

namespace scone
{
	class ExampleObjective : public Objective
	{
	public:
		ExampleObjective( const PropNode& props );

	protected:
		virtual double evaluate( const ParamInstance& values ) const override;

	private:
		int num_params;
		mutable bool is_evaluating;
	};
}

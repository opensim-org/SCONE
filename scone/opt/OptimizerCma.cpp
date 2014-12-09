#include "stdafx.h"
#include "OptimizerCma.h"

namespace scone
{
	namespace opt
	{
		OptimizerCma::OptimizerCma() :
		mu( 0.0 ),
		lambda( 0.0 ),
		sigma( 1.0 ),
		max_generations( 10000 )
		{
		}

		OptimizerCma::~OptimizerCma()
		{
		}

		void OptimizerCma::ProcessProperties( const PropNode& props )
		{
			Optimizer::ProcessProperties( props );

			PROCESS_PROPERTY( props, lambda );
			PROCESS_PROPERTY( props, mu );
			PROCESS_PROPERTY( props, sigma );
			PROCESS_PROPERTY( props, max_generations );
		}
	}
}

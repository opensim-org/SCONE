#pragma once

#include "CmaOptimizer.h"

namespace scone
{
	namespace opt
	{
		class SCONE_API CmaOptimizerCCMAES : public CmaOptimizer
		{
		public:
			CmaOptimizerCCMAES( const PropNode& props );
			virtual ~CmaOptimizerCCMAES() {}

			virtual void Run() override;
		};
	}
}

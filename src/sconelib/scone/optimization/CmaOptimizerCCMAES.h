#pragma once

#include "CmaOptimizer.h"

namespace scone
{
	class SCONE_API CmaOptimizerCCMAES : public CmaOptimizer
	{
	public:
		CmaOptimizerCCMAES( const PropNode& props );
		virtual ~CmaOptimizerCCMAES() {}

		virtual void Run() override;
	};
}

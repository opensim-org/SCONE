#pragma once

#ifdef USE_SHARK_2

#include "CmaOptimizer.h"

namespace scone
{
	namespace opt
	{
		class SCONE_API CmaOptimizerShark2 : public CmaOptimizer
		{
		public:
			CmaOptimizerShark2( const PropNode& props );
			virtual ~CmaOptimizerShark2() {}

			virtual void Run() override;
		};
	}
}

#endif
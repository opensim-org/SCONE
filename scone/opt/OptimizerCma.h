#pragma once

#include "opt.h"
#include "Optimizer.h"

namespace scone
{
	namespace opt
	{
		class OPT_API OptimizerCma : public Optimizer
		{
		public:
			OptimizerCma();
			virtual ~OptimizerCma();
			
		protected:
		private:
		};
	}
}

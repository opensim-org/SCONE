#pragma once
#include "opt.h"
#include "Objective.h"

namespace scone
{
	namespace opt
	{
		class OPT_API Optimizer
		{
		public:
			Optimizer( Objective& obj );
			virtual ~Optimizer();
			
		private:
			Objective& m_Objective;
		};
	}
}

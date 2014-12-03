#pragma once

#include "cs.h"
#include "..\opt\opt.h"
#include "..\core\core.h"

namespace scone
{
	namespace cs
	{
		class CS_API Optimization
		{
		public:
			Optimization();
			virtual ~Optimization();

			void Run( const String& script_file );
			
		private:
			opt::OptimizerSP m_Optimizer;
			opt::ObjectiveSP m_Objective;
		};
	}
}

#pragma once

#include "cs.h"
#include "factory.h"
#include "..\opt\opt.h"
#include "..\core\core.h"
#include "..\core\Propertyable.h"
#include <string>

namespace scone
{
	namespace cs
	{
		class CS_API Optimization : public Propertyable
		{
		public:
			Optimization();
			virtual ~Optimization();

			void Run( const String& script_file );

			virtual void ProcessProperties( const PropNode& props ) override;

		private:
			Objective m_Objective;
			opt::OptimizerSP m_Optimizer;
			std::string m_Test;
		};
	}
}

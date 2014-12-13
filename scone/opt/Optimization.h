#pragma once

#include "opt.h"
#include "..\core\Propertyable.h"
#include "Optimizer.h"
#include "Objective.h"

namespace scone
{
	namespace opt
	{
		class OPT_API Optimization : public Propertyable
		{
		public:
			Optimization();
			virtual ~Optimization();

			void Run( const String& script_file );

			virtual void ProcessProperties( const PropNode& props ) override;

		private:
			std::shared_ptr< Optimizer > m_Optimizer;
		};
	}
}

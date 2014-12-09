#pragma once
#include "opt.h"
#include "Objective.h"
#include "..\core\Propertyable.h"

namespace scone
{
	namespace opt
	{
		class OPT_API Optimizer : public Propertyable
		{
		public:
			Optimizer();
			virtual ~Optimizer();

			virtual void ProcessProperties( const PropNode& props ) override;


		private:
		};
	}
}

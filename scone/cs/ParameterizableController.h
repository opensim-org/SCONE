#pragma once
#include "../sim/Controller.h"
#include "../opt/ParamSet.h"

namespace scone
{
	namespace cs
	{
		class CS_API ParameterizableController : public sim::Controller
		{
		public:
			ParameterizableController() { };
			virtual ~ParameterizableController() { };
			
		protected:
		private:
		};
	}
}

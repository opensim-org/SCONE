#pragma once

#include "sim.h"

namespace scone
{
	namespace sim
	{
		class SIM_API Controller
		{
		public:
			Controller() { };
			virtual ~Controller() { };
			
			virtual void SetModel( ModelSharedPtr model ) = 0;
			virtual ModelSharedPtr GetModel() = 0;

			virtual void Update( double timestamp ) = 0;

		protected:
		private:
		};
	}
}

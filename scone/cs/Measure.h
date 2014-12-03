#pragma once
#include "..\sim\sim.h"

namespace scone
{
	namespace cs
	{
		class CS_API Measure
		{
		public:
			Measure() { };
			virtual ~Measure() { };

			virtual bool Update( sim::ModelSP model, double timestamp ) = 0;
			
		private:
		};
	}
}

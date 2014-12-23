#pragma once
#include "..\sim\sim.h"
#include "..\core\Propertyable.h"
#include "..\sim\Controller.h"

namespace scone
{
	namespace cs
	{
		class Measure : public sim::Controller
		{
		public:
			Measure() { };
			virtual ~Measure() { };

			virtual bool UpdateControls( sim::Model& model, double timestamp ) = 0;
			virtual void ProcessProperties( const PropNode& props ) override;

			virtual double GetValue() = 0;

		private:
		};
	}
}

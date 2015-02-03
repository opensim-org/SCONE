#pragma once

#include "../sim/Controller.h"
#include "../core/Factory.h"

namespace scone
{
	namespace cs
	{
		class FiniteStateMachine : public sim::Controller, public Factoryable< sim::Controller, FiniteStateMachine >
		{
		public:
			FiniteStateMachine( const PropNode& props );
			virtual ~FiniteStateMachine();
			

		};
	}
}

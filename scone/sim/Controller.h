#pragma once

#include "sim.h"
#include "../core/Propertyable.h"
#include <functional>
#include "../opt/ParamSet.h"

namespace scone
{
	namespace sim
	{
		class SCONE_SIM_API Controller : public Propertyable, public opt::Parameterizable
		{
		public:
			Controller();
			virtual ~Controller();

			virtual void ConnectModel( Model& model ) { };
			virtual bool UpdateControls( Model& model, double timestamp ) = 0;

			virtual void ProcessProperties( const PropNode& props );
		};
	}
}

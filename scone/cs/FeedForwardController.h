#pragma once
#include "../sim/Controller.h"
#include "cs.h"
#include "../core/PropNode.h"
#include "../opt/ParamSet.h"
#include "ParameterizableController.h"

namespace scone
{
	namespace cs
	{
		class CS_API FeedForwardController : public ParameterizableController
		{
		public:
			FeedForwardController() { };
			virtual ~FeedForwardController() { };

			virtual void ProcessPropNode( PropNode& props ) override;

			virtual void ProcessParamSet( opt::ParamSet& par ) override;

			virtual bool RegisterModel( sim::ModelSP model ) override;

			virtual void UnregisterModel( sim::ModelSP model ) override;

			virtual bool Update( sim::ModelSP model, double timestamp ) override;
		};
	}
}

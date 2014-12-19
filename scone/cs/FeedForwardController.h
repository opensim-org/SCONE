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
		class CS_API FeedForwardController : public ParameterizableController, public Factoryable< sim::Controller, FeedForwardController >
		{
		public:
			FeedForwardController() { };
			virtual ~FeedForwardController() { };

			virtual void ProcessProperties( const PropNode& props ) override;

			virtual void ProcessParameters( opt::ParamSet& par ) override;

			virtual bool RegisterModel( sim::ModelSP model ) override;

			virtual void UnregisterModel( sim::ModelSP model ) override;

			virtual bool Update( double timestamp ) override;
		};
	}
}

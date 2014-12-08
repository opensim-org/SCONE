#include "stdafx.h"
#include "FeedForwardController.h"
#include "..\sim\sim.h"

namespace scone
{
	namespace cs
	{
		void FeedForwardController::ProcessPropNode( PropNode& props )
		{
			throw std::logic_error("The method or operation is not implemented.");
		}

		void FeedForwardController::ProcessParamSet( opt::ParamSet& par )
		{
			throw std::logic_error("The method or operation is not implemented.");
		}

		bool FeedForwardController::RegisterModel( sim::ModelSP model )
		{
			throw std::logic_error("The method or operation is not implemented.");
		}

		void FeedForwardController::UnregisterModel( sim::ModelSP model )
		{
			throw std::logic_error("The method or operation is not implemented.");
		}

		bool FeedForwardController::Update( sim::ModelSP model, double timestamp )
		{
			throw std::logic_error("The method or operation is not implemented.");
		}
	}
}

#include "stdafx.h"
#include "ReflexController.h"

namespace scone
{
	namespace cs
	{
		ReflexController::ReflexController( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& target_area ) :
		sim::Controller( props, par, model, target_area )
		{
		}

		ReflexController::~ReflexController()
		{
		}

		void ReflexController::UpdateControls( sim::Model& model, double timestamp )
		{
			throw std::logic_error("The method or operation is not implemented.");
		}

		scone::String ReflexController::GetSignature()
		{
			throw std::logic_error("The method or operation is not implemented.");
		}

	}
}

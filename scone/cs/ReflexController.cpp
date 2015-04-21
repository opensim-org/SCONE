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
	}
}

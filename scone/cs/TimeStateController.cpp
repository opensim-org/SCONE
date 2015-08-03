#include "stdafx.h"
#include "TimeStateController.h"

namespace scone
{
	namespace cs
	{
		TimeStateController::TimeStateController( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area ) :
		Controller( props, par, model, area )
		{
		}

		TimeStateController::~TimeStateController()
		{
		}
	}
}

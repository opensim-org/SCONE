#include "stdafx.h"
#include "JumpMeasure.h"
#include "../sim/Area.h"

namespace scone
{
	namespace cs
	{
		JumpMeasure::JumpMeasure( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area ) :
		Measure( props, par, model, area )
		{
		}

		JumpMeasure::~JumpMeasure()
		{
		}
	}
}

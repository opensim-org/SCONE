#pragma once

#include "cs.h"
#include "Measure.h"

#include "../core/PropNode.h"
#include "../opt/ParamSet.h"
#include "../sim/Model.h"

namespace scone
{
	namespace cs
	{
		class CS_API JumpMeasure : public Measure
		{
		public:
			JumpMeasure( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area );
			virtual ~JumpMeasure();
			
		protected:
		private:
		};
	}
}

#pragma once

#include "cs.h"
#include "../sim/Controller.h"
#include "../core/PropNode.h"
#include "../opt/ParamSet.h"
#include "../sim/Model.h"

namespace scone
{
	namespace cs
	{
		class CS_API TimeStateController : public sim::Controller
		{
		public:
			TimeStateController( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area );
			virtual ~TimeStateController();
			
		protected:
		private:
		};
	}
}

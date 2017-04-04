#pragma once

#include "cs.h"

namespace scone
{
	namespace cs
	{
		class CS_API MetaReflexPose
		{
		public:
			MetaReflexPose( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area );
			virtual ~MetaReflexPose() {}
			
		private:
		};
	}
}

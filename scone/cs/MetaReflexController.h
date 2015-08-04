#pragma once

#include "../sim/Controller.h"
#include "cs.h"

namespace scone
{
	namespace cs
	{
		class CS_API MetaReflexController : public sim::Controller
		{
		public:
			MetaReflexController( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area );
			virtual ~MetaReflexController();
			
			virtual UpdateResult UpdateControls( sim::Model& model, double timestamp ) override;

		private:
			MetaReflexController( const MetaReflexController& other );
			MetaReflexController& operator=( const MetaReflexController& other );

			std::vector< MetaReflexUP > m_Reflexes;
		};
	}
}

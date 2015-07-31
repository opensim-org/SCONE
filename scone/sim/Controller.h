#pragma once

#include "sim.h"
#include <functional>
#include "../opt/ParamSet.h"
#include "../core/HasSignature.h"

namespace scone
{
	namespace sim
	{
		class SCONE_SIM_API Controller : public HasSignature
		{
		public:
			Controller( const PropNode& props, opt::ParamSet& par, sim::Model& model, const Area& target_area );
			virtual ~Controller();

			enum UpdateResult { SuccessfulUpdate, NoUpdate, RequestTermination };

			/// Called each attempted integration step
			virtual UpdateResult UpdateControls( sim::Model& model, double timestamp ) { return NoUpdate; }

			/// Called after each successful integration step
			virtual UpdateResult UpdateAnalysis( const sim::Model& model, double timestamp ) { return NoUpdate; }

		private:
			bool m_TerminationRequest;
		};
	}
}

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

			virtual UpdateResult UpdateControls( sim::Model& model, double timestamp ) { return NoUpdate; }
			virtual UpdateResult UpdateAnalysis( sim::Model& model, double timestamp ) { return NoUpdate; }

		private:
			bool m_TerminationRequest;
		};
	}
}

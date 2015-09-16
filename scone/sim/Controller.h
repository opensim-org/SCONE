#pragma once

#include "sim.h"
#include <functional>
#include "../opt/ParamSet.h"
#include "../core/HasSignature.h"
#include "../core/HasData.h"

namespace scone
{
	namespace sim
	{
		class SCONE_SIM_API Controller : public HasSignature, public HasData
		{
		public:
			Controller( const PropNode& props, opt::ParamSet& par, sim::Model& model, const Area& target_area );
			virtual ~Controller();

			enum UpdateResult { SuccessfulUpdate, NoUpdate, RequestTermination };

			/// Called each attempted integration step
			// TODO: use Result< UpdateResult > or bool
			virtual UpdateResult UpdateControls( sim::Model& model, double timestamp ) { return NoUpdate; }

			/// Called after each successful integration step
			// TODO: use Result< UpdateResult > or bool
			virtual UpdateResult UpdateAnalysis( const sim::Model& model, double timestamp ) { return NoUpdate; }

			// default implementation doesn't store anything
			virtual void StoreData( Storage< Real >::Frame& frame ) override {}

		private:
			bool m_TerminationRequest;
		};
	}
}

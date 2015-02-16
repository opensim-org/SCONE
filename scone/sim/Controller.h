#pragma once

#include "sim.h"
#include <functional>
#include "../opt/ParamSet.h"

namespace scone
{
	namespace sim
	{
		class SCONE_SIM_API Controller
		{
		public:
			Controller( const PropNode& props, opt::ParamSet& par, sim::Model& model );
			virtual ~Controller();

			virtual void UpdateControls( sim::Model& model, double timestamp ) = 0;

			void SetTerminationRequest( bool value = true ) { m_TerminationRequest = value; }
			bool GetTerminationRequest() { return m_TerminationRequest; }

		private:
			bool m_TerminationRequest;
		};
	}
}

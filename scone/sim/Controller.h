#pragma once

#include "sim.h"
#include <functional>
#include "../opt/ParamSet.h"

namespace scone
{
	namespace sim
	{
		class SCONE_SIM_API Controller : public opt::Parameterizable
		{
		public:
			Controller( const PropNode& props );
			virtual ~Controller();

			//virtual void Initialize( sim::Model& model, opt::ParamSet& par, const PropNode& props );
			virtual void Initialize( sim::Model& model );
			virtual void UpdateControls( sim::Model& model, double timestamp ) = 0;

			void SetTerminationRequest( bool value = true ) { m_TerminationRequest = value; }
			bool GetTerminationRequest() { return m_TerminationRequest; }

		private:
			bool m_TerminationRequest;
		};
	}
}

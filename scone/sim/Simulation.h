#pragma once

#include "sim.h"
#include "Model.h"
#include <memory>

#include "../core/PropNode.h"
#include "../opt/ParamSet.h"

namespace scone
{
	namespace sim
	{
		class SCONE_SIM_API Simulation
		{
		public:
			Simulation( const PropNode& props );
			virtual ~Simulation();

			Model& AddModel( const String& filename );
			size_t GetModelCount() { return m_Models.size(); }
			Model& GetModel( size_t idx = 0 );

			virtual void AdvanceSimulationTo( double time ) = 0;

			double max_simulation_time;
			double integration_accuracy;

		protected:
			std::vector< ModelUP > m_Models;

		private: // non-copyable and non-assignable
			Simulation( const Simulation& );
			Simulation& operator=( const Simulation& );
		};
	}
}

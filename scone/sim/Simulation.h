#pragma once

#include "sim.h"
#include "Model.h"
#include <memory>
#include "..\core\PropNode.h"
#include "..\opt\ParamSet.h"

namespace scone
{
	namespace sim
	{
		class SCONE_SIM_API Simulation : public Propertyable, public opt::Parameterizable
		{
		public:
			Simulation();
			virtual ~Simulation();

			Model& AddModel( const String& filename );
			size_t GetModelCount() { return m_Models.size(); }
			Model& GetModel( size_t idx = 0 );

			virtual void ProcessProperties( const PropNode& props );
			virtual void AdvanceSimulationTo( double time ) = 0;

			double max_simulation_time;

			virtual void ProcessParameters( opt::ParamSet& par ) override;

		protected:
			virtual ModelUP CreateModel( const String& filename ) = 0;
			std::vector< ModelUP > m_Models;

		private: // non-copyable and non-assignable
			Simulation( const Simulation& );
			Simulation& operator=( const Simulation& );
		};
	}
}

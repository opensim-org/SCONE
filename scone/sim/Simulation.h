#pragma once

#include "sim.h"
#include "Model.h"
#include <memory>
#include "..\core\PropNode.h"

namespace scone
{
	namespace sim
	{
		class SCONE_SIM_API Simulation : public Propertyable
		{
		public:
			Simulation();
			virtual ~Simulation();
			virtual ModelSP AddModel();
			virtual ModelSP GetModel( size_t idx = 0 );
			virtual void ProcessProperties( const PropNode& props );

			virtual void Run() = 0;

		protected:
			virtual ModelSP CreateModel() = 0;

		private:
			std::vector< ModelSP > m_Models;
		};
	}
}

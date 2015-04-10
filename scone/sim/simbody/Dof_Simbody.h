#pragma once

#include "sim_simbody.h"
#include "../Dof.h"

namespace OpenSim
{
	class Coordinate;
}

namespace scone
{
	namespace sim
	{
		class SCONE_SIM_SIMBODY_API Dof_Simbody : public Dof
		{
		public:
			Dof_Simbody( class Model_Simbody& model, OpenSim::Coordinate& coord );
			virtual ~Dof_Simbody();
			
		private:
			Model_Simbody& m_Model;
			OpenSim::Coordinate& m_osCoord;
		};
	}
}

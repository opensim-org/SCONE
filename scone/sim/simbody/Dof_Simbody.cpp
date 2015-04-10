#include "stdafx.h"

#include "Dof_Simbody.h"
#include "OpenSim/Simulation/SimbodyEngine/Coordinate.h"
#include "Model_Simbody.h"

namespace scone
{
	namespace sim
	{
		Dof_Simbody::Dof_Simbody( class Model_Simbody& model, OpenSim::Coordinate& coord ) :
		m_Model( model ),
		m_osCoord( coord )
		{

		}

		Dof_Simbody::~Dof_Simbody()
		{

		}
	}
}

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

		scone::Real Dof_Simbody::GetPos()
		{
			return m_osCoord.getValue( m_Model.GetTkState() );
		}

		scone::Real Dof_Simbody::GetVel()
		{
			return m_osCoord.getSpeedValue( m_Model.GetTkState() );
		}

		const String& Dof_Simbody::GetName() const 
		{
			return m_osCoord.getName();
		}
	}
}

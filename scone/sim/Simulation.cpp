#include "stdafx.h"
#include "Simulation.h"

namespace scone
{
	namespace sim
	{
		Simulation::Simulation()
		{

		}

		Simulation::~Simulation()
		{

		}

		void Simulation::ProcessProperties( const PropNode& props )
		{

		}

		scone::sim::ModelSP Simulation::AddModel()
		{
			ModelSP m = CreateModel();
			m_Models.push_back( m );
			return m;
		}

		scone::sim::ModelSP Simulation::GetModel( size_t idx )
		{
			SCONE_ASSERT( idx < m_Models.size() );
			return m_Models[ idx ];
		}
	}
}

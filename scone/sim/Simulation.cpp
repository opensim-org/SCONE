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

		Model& Simulation::AddModel()
		{
			m_Models.emplace_back( CreateModel() );
			return *m_Models.back();
		}

		Model& Simulation::GetModel( size_t idx )
		{
			SCONE_ASSERT( idx < m_Models.size() );
			return *m_Models[ idx ];
		}
	}
}

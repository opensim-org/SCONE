#include "stdafx.h"

#include "SimulationObjective.h"
#include "..\core\Exception.h"

namespace scone
{
	namespace cs
	{
		SimulationObjective::SimulationObjective()
		{
		}

		SimulationObjective::~SimulationObjective()
		{
		}

		double SimulationObjective::Evaluate()
		{
			sim::SimulationSP s;
			ParameterizableControllerSP c;
			MeasureSP m;

			ProcessPropNode( m_Props, s, "Simulation" );
			ProcessPropNode( m_Props, c, "Controller" );
			ProcessPropNode( m_Props, m, "Measure" );

			c->ProcessParameters( m_Params );

			s->GetModel().AddController( c );
			s->GetModel().AddController( m );

			s->Run();

			return m->GetValue();
		}

		void SimulationObjective::ProcessProperties( const PropNode& props )
		{
			// just copy properties
			m_Props = props;
		}

		void SimulationObjective::ProcessParameters( opt::ParamSet& par )
		{
			// just copy parameters
			m_Params = par;
		}

	}
}

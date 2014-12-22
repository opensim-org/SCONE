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

			InitFromPropNode( m_Props, s, "Simulation" );
			InitFromPropNode( m_Props, c, "Controller" );
			InitFromPropNode( m_Props, m, "Measure" );

			c->ProcessParameters( m_Params );

			s->GetModel().AddController( c );
			s->GetModel().AddController( m );

			s->AdvanceSimulationTo( s->max_simulation_time );

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

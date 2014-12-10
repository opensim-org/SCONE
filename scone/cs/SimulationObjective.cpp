#include "stdafx.h"
#include "SimulationObjective.h"
#include "..\core\Exception.h"
#include "factory.h"

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
			sim::SimulationSP s( factory::Create< sim::Simulation >( m_SimulationProps ) );
			ParameterizableControllerSP c( factory::Create< cs::ParameterizableController >( m_ControllerProps ) );
			MeasureSP m( factory::Create< cs::Measure >( m_MeasureProps ) );

			s->GetModel()->AddController( c );
			s->GetModel()->AddController( m );

			s->Run();

			return m->GetValue();
		}

		void SimulationObjective::ProcessProperties( const PropNode& props )
		{
			// copy properties
			m_SimulationProps = *props.GetChildPtr( "Simulation" );
			m_ControllerProps = *props.GetChildPtr( "Controller" );
			m_MeasureProps = *props.GetChildPtr( "Measure" );
		}

		void SimulationObjective::ProcessParameters( opt::ParamSet& par )
		{
			throw std::logic_error("The method or operation is not implemented.");
		}

	}
}

#include "stdafx.h"
#include "Objective.h"
#include "..\core\Exception.h"
#include "factory.h"

namespace scone
{
	namespace cs
	{
		Objective::Objective()
		{
		}

		Objective::~Objective()
		{
		}

		opt::ParamSet Objective::GetParamSet()
		{
			opt::ParamSet par( opt::ParamSet::CONSTRUCTION_MODE );

			ParameterizableControllerSP c = factory::CreateSP< cs::ParameterizableController >( m_ControllerProps );

			c->ProcessParamSet( par );

			return par;
		}

		double Objective::Evaluate( const opt::ParamSet& params )
		{
			sim::SimulationSP s = factory::CreateSP< sim::Simulation >( m_SimulationProps );
			ParameterizableControllerSP c = factory::CreateSP< cs::ParameterizableController >( m_ControllerProps );
			MeasureSP m = factory::CreateSP< cs::Measure >( m_MeasureProps );

			s->GetModel()->AddController( c );
			s->GetModel()->AddController( m );

			s->Run();

			return m->GetValue();
		}

		void Objective::ProcessPropNode( PropNode& props )
		{
			// copy properties
			m_SimulationProps = *props.GetChildPtr( "Simulation" );
			m_ControllerProps = *props.GetChildPtr( "Controller" );
			m_MeasureProps = *props.GetChildPtr( "Measure" );
		}
	}
}

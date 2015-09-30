#include "stdafx.h"

#include "SimulationObjective.h"
#include "../core/Exception.h"

#include <boost/foreach.hpp>
#include <algorithm>

#include "../sim/Model.h"
#include "../sim/Factories.h"

#include <boost/thread.hpp>
#include "version.h"
#include "../core/Profiler.h"
#include "../core/system.h"

namespace scone
{
	namespace cs
	{
		SimulationObjective::SimulationObjective( const PropNode& props, opt::ParamSet& par ) :
		Objective( props, par ),
		m_ModelProps( props.GetChild( "Model" ) )
		{
			INIT_PROPERTY( props, max_duration, 6000.0 );

			ProcessParameters( par );
		}

		SimulationObjective::~SimulationObjective()
		{
		}

		double SimulationObjective::Evaluate()
		{
			SCONE_PROFILE_SCOPE;
			SCONE_ASSERT( m_Model && m_Measure );

			// run the simulation
			m_Model->AdvanceSimulationTo( max_duration );
			return m_Measure->GetResult( *m_Model );
		}

		void SimulationObjective::ProcessParameters( opt::ParamSet& par )
		{
			// (re)create new model using stored model props
			m_Model = sim::CreateModel( m_ModelProps, par );

			// find measure controller
			auto& controllers = m_Model->GetControllers();
			auto& is_measure = [&]( sim::ControllerUP& c ) { return dynamic_cast<Measure*>( c.get() ) != nullptr; };
			auto measureIter = std::find_if( controllers.begin(), controllers.end(), is_measure );

			if ( measureIter == controllers.end() )
				SCONE_THROW( "Could not find a measure" );
			else if ( controllers.end() != std::find_if( measureIter + 1, controllers.end(), is_measure ) )
				SCONE_THROW( "More than one measure was found" );

			m_Measure = dynamic_cast< Measure* >( measureIter->get() );
		}

		std::vector< String > SimulationObjective::WriteResults( const String& file )
		{
			std::vector< String > files;
			files.push_back( m_Model->WriteData( file ) );
			return files;
		}

		String SimulationObjective::GetClassSignature() const
		{
			String str = GetApplicationVersion() + "." + m_Model->GetSignature();

			BOOST_FOREACH( sim::ControllerUP& c, m_Model->GetControllers() )
				str += "." + c->GetSignature();
			str += GetStringF( ".D%.0f", max_duration );

			return str;
		}
	}
}

#include "SimulationObjective.h"
#include "scone/core/Exception.h"

#include <algorithm>

#include "scone/model/Model.h"

#include "scone/core/version.h"
#include "scone/core/Profiler.h"
#include "scone/core/system_tools.h"
#include "scone/core/Factories.h"

#include <boost/thread.hpp>

namespace scone
{
	SimulationObjective::SimulationObjective( const PropNode& props, ParamSet& par ) :
	Objective( props, par ),
	m_ModelProps( props.get_child( "Model" ) )
	{
		INIT_PROPERTY( props, max_duration, 6000.0 );

		// process parameters to flag unused model props
		ProcessParameters( par );

		// populate par_info_
		for ( size_t par_idx = 0; par_idx < par.GetParamCount(); ++par_idx )
		{
			auto& parinf = par.GetParamInfo( par_idx );
			if ( parinf.is_free )
				par_info_.push_back( parinf.name, parinf.init_mean, parinf.init_std, parinf.min, parinf.max );
		}
	}

	SimulationObjective::~SimulationObjective()
	{
	}

	double SimulationObjective::Evaluate()
	{
		SCONE_PROFILE_FUNCTION;
		SCONE_ASSERT( m_Model && m_Measure );

		// run the simulation
		m_Model->SetSimulationEndTime( max_duration );
		m_Model->AdvanceSimulationTo( max_duration );
		return m_Measure->GetResult( *m_Model );
	}

	void SimulationObjective::ProcessParameters( ParamSet& par )
	{
		// (re)create new model using stored model props
		m_Model = CreateModel( m_ModelProps, par );

		// find measure controller
		auto& controllers = m_Model->GetControllers();
		const auto& is_measure = [&]( ControllerUP& c ) { return dynamic_cast<Measure*>( c.get() ) != nullptr; };
		auto measureIter = std::find_if( controllers.begin(), controllers.end(), is_measure );

		if ( measureIter == controllers.end() )
			SCONE_THROW( "Could not find a measure" );
		else if ( controllers.end() != std::find_if( measureIter + 1, controllers.end(), is_measure ) )
			SCONE_THROW( "More than one measure was found" );

		m_Measure = dynamic_cast<Measure*>( measureIter->get() );
	}

	std::vector< String > SimulationObjective::WriteResults( const String& file )
	{
		std::vector< String > files;
		files.push_back( m_Model->WriteData( file ) );
		return files;
	}

	String SimulationObjective::GetClassSignature() const
	{
		String str = m_Model->GetSignature();

		for ( ControllerUP& c : m_Model->GetControllers() )
			str += "." + c->GetSignature();
		str += stringf( ".D%.0f", max_duration );

		return str;
	}
}

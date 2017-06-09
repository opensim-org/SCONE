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
	SimulationObjective::SimulationObjective( const PropNode& props ) :
	Objective( props ),
	m_ModelProps( props.get_child( "Model" ) )
	{
		INIT_PROPERTY( props, max_duration, 6000.0 );

		// create model to flag unused model props and create par_info_
		CreateModelFromParameters( info_ );
	}

	SimulationObjective::~SimulationObjective()
	{}

	void SimulationObjective::CreateModelFromParameters( Params& par )
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

		m_Measure = dynamic_cast< Measure* >( measureIter->get() );
	}

	std::vector< String > SimulationObjective::WriteResults( const String& file )
	{
		std::vector< String > files;
		files.push_back( m_Model->WriteData( file ) );
		return files;
	}

	scone::fitness_t SimulationObjective::evaluate( const flut::par_vec& point ) const
	{
		// WARNING: this function is thread-safe and should only access local variables
		auto model = CreateModel( m_ModelProps, ParamInstance( info(), point ) );
		auto& controllers = model->GetControllers();
		const auto& is_measure = [&]( ControllerUP& c ) { return dynamic_cast<Measure*>( c.get() ) != nullptr; };
		auto measureIter = std::find_if( controllers.begin(), controllers.end(), is_measure );

		if ( measureIter == controllers.end() )
			SCONE_THROW( "Could not find a measure" );
		else if ( controllers.end() != std::find_if( measureIter + 1, controllers.end(), is_measure ) )
			SCONE_THROW( "More than one measure was found" );
		auto measue = dynamic_cast<Measure*>( measureIter->get() );

		// run the simulation
		model->SetSimulationEndTime( max_duration );
		model->AdvanceSimulationTo( max_duration );
		return measue->GetResult( *model );
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

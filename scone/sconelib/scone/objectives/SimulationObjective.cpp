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
	m_ModelPropsCopy( props.get_child( "Model" ) )
	{
		INIT_PROPERTY( props, max_duration, 6000.0 );

		// create model to flag unused model props and create par_info_
		auto model = CreateModel( props.get_child( "Model" ), info_ );
		m_Signature = model->GetSignature() + stringf( ".D%.0f", max_duration );
	}

	SimulationObjective::~SimulationObjective()
	{}

	ModelUP SimulationObjective::CreateModelFromParameters( Params& par ) const
	{
		return CreateModel( m_ModelPropsCopy, par );
	}

	scone::ModelUP SimulationObjective::CreateModelFromParFile( const path& parfile ) const
	{
		return CreateModel( m_ModelPropsCopy, ParamInstance( info_, parfile ) );
	}

	scone::fitness_t SimulationObjective::evaluate( const ParamInstance& point ) const
	{
		// WARNING: this function is thread-safe and should only access local or const variables
		auto model = CreateModel( m_ModelPropsCopy, ParamInstance( point ) );

		// run the simulation
		return EvaluateModel( *model );
	}

	scone::fitness_t SimulationObjective::EvaluateModel( Model& model ) const
	{
		model.SetSimulationEndTime( max_duration );
		model.AdvanceSimulationTo( max_duration );
		return model.GetMeasure()->GetResult( model );
	}

	String SimulationObjective::GetClassSignature() const
	{
		return m_Signature;
	}
}

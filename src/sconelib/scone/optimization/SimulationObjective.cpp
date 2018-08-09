#include "SimulationObjective.h"

#include "scone/core/Exception.h"
#include "scone/model/Model.h"

#include "scone/core/version.h"
#include "scone/core/string_tools.h"
#include "scone/core/system_tools.h"
#include "scone/core/Factories.h"

namespace scone
{
	SimulationObjective::SimulationObjective( const PropNode& props ) :
	ModelObjective( props )
	{
		INIT_PROPERTY( props, max_duration, 6000.0 );

		// create model to flag unused model props and create par_info_
		auto model = CreateModel( props.get_child( "Model" ), info_ );

		// create a measure that's defined OUTSIDE the model prop_node
		if ( auto mp = props.try_get_child( "Measure" ) )
		{
			m_MeasurePropsCopy = *mp;
			model->SetMeasure( CreateMeasure( *mp, info_, *model, Locality( NoSide ) ) );
		}

		SCONE_THROW_IF( !model->GetMeasure(), "No Measure defined" );

		info_.set_minimize( model->GetMeasure()->GetMinimize() );
		signature_ = model->GetSignature() + stringf( ".D%.0f", max_duration );
		AddExternalResources( model->GetExternalResources() );
	}

	SimulationObjective::~SimulationObjective()
	{}

	scone::fitness_t SimulationObjective::EvaluateModel( Model& m ) const
	{
		m.SetSimulationEndTime( GetDuration() );
		AdvanceModel( m, GetDuration() );
		return m.GetMeasure()->GetResult( m );
	}

	void SimulationObjective::AdvanceModel( Model& m, TimeInSeconds t ) const
	{
		m.AdvanceSimulationTo( t );
	}

	scone::ModelUP SimulationObjective::CreateModelFromParams( Params& point ) const
	{
		auto model = CreateModel( m_ModelPropsCopy, point );

		if ( !m_MeasurePropsCopy.empty() ) // A measure was defined OUTSIDE the model prop_node
			model->SetMeasure( CreateMeasure( m_MeasurePropsCopy, point, *model, Locality( NoSide ) ) );
		return model;
	}

	String SimulationObjective::GetClassSignature() const
	{
		return signature_;
	}
}

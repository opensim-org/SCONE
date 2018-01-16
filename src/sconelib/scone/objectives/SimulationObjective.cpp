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
		signature_ = model->GetSignature() + stringf( ".D%.0f", max_duration );
		append( external_files_, model->GetExternalFiles() );
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

	String SimulationObjective::GetClassSignature() const
	{
		return signature_;
	}
}

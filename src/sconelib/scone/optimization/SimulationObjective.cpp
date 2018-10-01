/*
** SimulationObjective.cpp
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

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
		INIT_PROP( props, max_duration, 1e12 );

		// create model using the ORIGINAL porp_node to flag unused model props and create par_info_
		auto m = CreateModel( props.get_any_child( { "Model", "model" } ), info_ );

		// create a measure that's defined OUTSIDE the model prop_node
		if ( auto mp = props.try_get_any_child( { "Measure", "measure" } ) )
		{
			measure = *mp;
			m->SetMeasure( CreateMeasure( *mp, info_, *m, Location( NoSide ) ) );
		}

		SCONE_THROW_IF( !m->GetMeasure(), "No Measure defined" );

		info_.set_minimize( m->GetMeasure()->GetMinimize() );
		signature_ = m->GetSignature() + stringf( ".D%.0f", max_duration );
		AddExternalResources( m->GetExternalResources() );
	}

	SimulationObjective::~SimulationObjective()
	{}

	scone::fitness_t SimulationObjective::EvaluateModel( Model& m ) const
	{
		m.SetSimulationEndTime( GetDuration() );
		AdvanceModel( m, GetDuration() );
		return m.GetMeasure()->GetWeightedResult( m );
	}

	void SimulationObjective::AdvanceModel( Model& m, TimeInSeconds t ) const
	{
		m.AdvanceSimulationTo( t );
	}

	scone::ModelUP SimulationObjective::CreateModelFromParams( Params& point ) const
	{
		auto m = CreateModel( model, point );

		if ( !measure.empty() ) // A measure was defined OUTSIDE the model prop_node
			m->SetMeasure( CreateMeasure( measure, point, *m, Location( NoSide ) ) );
		return m;
	}

	String SimulationObjective::GetClassSignature() const
	{
		return signature_;
	}
}

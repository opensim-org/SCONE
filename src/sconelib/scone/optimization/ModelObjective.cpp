/*
** ModelObjective.cpp
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "ModelObjective.h"

#include "scone/core/Factories.h"
#include "scone/core/Log.h"
#include "xo/filesystem/filesystem.h"
#include "opt_tools.h"

namespace scone
{
	ModelObjective::ModelObjective( const PropNode& props ) :
	Objective( props )
	{
	}

	scone::fitness_t ModelObjective::evaluate( const SearchPoint& point ) const
	{
		auto model = CreateModelFromParams( SearchPoint( point ) );
		return EvaluateModel( *model );
	}

	scone::fitness_t ModelObjective::EvaluateModel( Model& m ) const
	{
		m.SetSimulationEndTime( GetDuration() );
		AdvanceSimulationTo( m, GetDuration() );
		return GetResult( m );
	}

	scone::ModelUP ModelObjective::CreateModelFromParams( Params& par ) const
	{
		auto model = CreateModel( model_props, par );

		if ( !controller_props.empty() ) // A controller was defined OUTSIDE the model prop_node
			model->SetController( CreateController( controller_props, par, *model, Location( NoSide ) ) );

		if ( !measure_props.empty() ) // A measure was defined OUTSIDE the model prop_node
			model->SetMeasure( CreateMeasure( measure_props, par, *model, Location( NoSide ) ) );

		return model;
	}

	scone::ModelUP ModelObjective::CreateModelFromParFile( const path& parfile ) const
	{
		return CreateModelFromParams( SearchPoint( info_, parfile ) );
	}

	std::vector<path> ModelObjective::WriteResults( const path & file_base )
	{
		// this does not work because we don't have a model member in Objective
		SCONE_THROW_NOT_IMPLEMENTED;
	}

	ModelUP ModelObjective::InitializeModelObjective( const PropNode& props )
	{
		// create TEMPORARY model using the ORIGINAL prop_node to flag unused model props and create par_info_
		ModelUP model;
		if ( auto* mprops = props.try_get_child( "Model" ) )
		{
			model = CreateModel( *mprops, info_ );
			model_props = *mprops;
		}

		SCONE_THROW_IF( !model, "No Model defined in ModelObjective" );

		// create a TEMPORARY controller that's defined OUTSIDE the model prop_node
		if ( auto* cprops = props.try_get_child( "Controller" ) )
		{
			model->SetController( CreateController( *cprops, info_, *model, Location( NoSide ) ) );
			controller_props = *cprops;
		}

		// create a TEMPORARY measure that's defined OUTSIDE the model prop_node
		if ( auto* mprops = props.try_get_child( "Measure" ) )
		{
			model->SetMeasure( CreateMeasure( *mprops, info_, *model, Location( NoSide ) ) );
			measure_props = *mprops;
		}

		SCONE_THROW_IF( !model->GetMeasure(), "No Measure defined in ModelObjective" );

		info_.set_minimize( model->GetMeasure()->GetMinimize() );
		signature_ = model->GetSignature();

		AddExternalResources( model->GetExternalResources() );

		return model;
	}

	SCONE_API ModelObjectiveUP CreateModelObjective( const path& file )
	{
		auto dir = file.parent_path();
		path scenario_file = FindScenario( file );

		// set current path to scenario path
		xo::current_path( scenario_file.parent_path() );

		// read properties
		PropNode configProp = xo::load_file_with_include( scenario_file, "INCLUDE" );
		PropNode& objProp = configProp.get_child( "Optimizer" ).get_child( "Objective" );

		// create SimulationObjective object
		auto mob = dynamic_unique_cast<ModelObjective>( CreateObjective( objProp ) );

		if ( file.extension() == "scone" )
		{
			// read mean / std from init file
			auto& optProp = configProp.get_child( "Optimizer" );
			if ( optProp.has_key( "init_file" ) && optProp.get< bool >( "use_init_file", true ) )
			{
				auto init_file = optProp.get< path >( "init_file" );
				auto result = mob->info().import_mean_std( init_file, optProp.get< bool >( "use_init_file_std", true ) );
				log::info( "Imported ", result.first, " of ", mob->dim(), ", skipped ", result.second, " parameters from ", init_file );
			}
		}

		// report unused properties
		if ( objProp.count_unaccessed() > 0 )
		{
			log::warning( "Warning, unused properties:" );
			xo::log_unaccessed( objProp );
		}

		return mob;
	}
}

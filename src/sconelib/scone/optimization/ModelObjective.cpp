/*
** ModelObjective.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
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
	{}

	fitness_t ModelObjective::evaluate( const SearchPoint& point ) const
	{
		SearchPoint params( point );
		auto model = CreateModelFromParams( params );
		return EvaluateModel( *model );
	}

	fitness_t ModelObjective::EvaluateModel( Model& m ) const
	{
		m.SetSimulationEndTime( GetDuration() );
		AdvanceSimulationTo( m, GetDuration() );
		return GetResult( m );
	}

	ModelUP ModelObjective::CreateModelFromParams( Params& par ) const
	{
		auto model = CreateModel( model_props, par );
		model->SetSimulationEndTime( GetDuration() );

		if ( controller_props ) // A controller was defined OUTSIDE the model prop_node
			model->SetController( CreateController( controller_props, par, *model, Location() ) );

		if ( measure_props ) // A measure was defined OUTSIDE the model prop_node
			model->SetMeasure( CreateMeasure( measure_props, par, *model, Location() ) );

		return model;
	}

	ModelUP ModelObjective::CreateModelFromParFile( const path& parfile ) const
	{
		SearchPoint params( info_ );
		auto result = params.import_values( parfile );
		log::info( "Read ", result.first, " of ", info().dim(), " parameters, skipped ", result.second, " from ", parfile.filename() );

		return CreateModelFromParams( params );
	}

	std::vector<path> ModelObjective::WriteResults( const path& file_base )
	{
		// this does not work because we don't have a model member in Objective
		SCONE_THROW_NOT_IMPLEMENTED;
	}

	void ModelObjective::InitializeModelObjective( const PropNode& props )
	{
		// create TEMPORARY model using the ORIGINAL prop_node to flag unused model props and create par_info_
		model_props = FindFactoryProps( GetModelFactory(), props, "Model" );
		model_ = CreateModel( model_props, info_ );

		// create a TEMPORARY controller that's defined OUTSIDE the model prop_node
		if ( controller_props = TryFindFactoryProps( GetControllerFactory(), props, "Controller" ) )
			model_->SetController( CreateController( controller_props, info_, *model_, Location() ) );

		// create a TEMPORARY measure that's defined OUTSIDE the model prop_node
		if ( measure_props = TryFindFactoryProps( GetMeasureFactory(), props, "Measure" ) )
			model_->SetMeasure( CreateMeasure( measure_props, info_, *model_, Location() ) );

		if ( model_->GetMeasure() )
			info_.set_minimize( model_->GetMeasure()->GetMinimize() );

		signature_ = model_->GetSignature();

		AddExternalResources( model_->GetExternalResources() );
	}

	ModelObjectiveUP CreateModelObjective( const PropNode& scenario_pn, const path& dir )
	{
		// set current path to scenario path
		current_find_file_folder( dir );

		// find objective
		FactoryProps opt_props = FindFactoryProps( GetOptimizerFactory(), scenario_pn, "Optimizer" );
		FactoryProps obj_props = FindFactoryProps( GetObjectiveFactory(), opt_props.props(), "Objective" );

		// create ModelObjective object
		auto mob = dynamic_unique_cast<ModelObjective>( CreateObjective( obj_props ) );

		// read mean / std from init file
		if ( opt_props.props().has_key( "init_file" ) && opt_props.props().get< bool >( "use_init_file", true ) )
		{
			auto init_file = opt_props.props().get< path >( "init_file" );
			auto result = mob->info().import_mean_std( init_file, opt_props.props().get< bool >( "use_init_file_std", true ) );
			log::debug( "Imported ", result.first, " of ", mob->dim(), ", skipped ", result.second, " parameters from ", init_file );
		}

		// report unused properties
		if ( obj_props.props().count_unaccessed() > 0 )
		{
			log::warning( "Warning, unused properties:" );
			xo::log_unaccessed( obj_props.props() );
		}

		return mob;
	}
}

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

namespace scone
{
	ModelObjective::ModelObjective( const PropNode& props ) :
	Objective( props ),
	model( props.get_any_child( { "Model", "model" } ) )
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
		return CreateModel( model, par );
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

	ModelObjectiveUP SCONE_API CreateModelObjective( const path& file )
	{
		bool is_par_file = file.extension() == "par";
		auto dir = file.parent_path();
		path scenario_file = is_par_file ? xo::find_file( { dir / "config.xml", dir / "config.scone" } ) : file;

		// set current path to scenario path
		xo::current_path( scenario_file.parent_path() );

		// read properties
		PropNode configProp = xo::load_file_with_include( scenario_file, "INCLUDE" );
		PropNode& objProp = configProp.get_child( "Optimizer" ).get_child( "Objective" );

		// create SimulationObjective object
		auto mob = dynamic_unique_cast<ModelObjective>( CreateObjective( objProp ) );

		if ( !is_par_file )
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

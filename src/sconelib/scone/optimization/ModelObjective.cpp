/*
** ModelObjective.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "ModelObjective.h"

#include "scone/core/Exception.h"
#include "scone/core/Factories.h"
#include "scone/core/Log.h"
#include "spot/spot_types.h"
#include "xo/filesystem/filesystem.h"
#include "opt_tools.h"
#include "scone/core/profiler_config.h"
#include <algorithm>

namespace scone
{
	ModelObjective::ModelObjective( const PropNode& props, const path& find_file_folder ) :
		Objective( props, find_file_folder ),
		evaluation_step_size_( XO_IS_DEBUG_BUILD ? 0.01 : 0.25 )
	{
		INIT_PROP ( props, num_evaluations_to_report, 1 );

		// should be positive number
		SCONE_THROW_IF( num_evaluations_to_report < 1,
						"num_evaluations_to_report > 0" );

		// create internal model using the ORIGINAL prop_node to flag unused model props and create par_info_
		model_props = FindFactoryProps( GetModelFactory(), props, "Model" );
		model_ = CreateModel( model_props, info_, GetExternalResourceDir() );

		// create a controller that's defined OUTSIDE the model prop_node
		if ( controller_props = TryFindFactoryProps( GetControllerFactory(), props, "Controller" ) )
			model_->CreateController( controller_props, info_ );

		// create a measure that's defined OUTSIDE the model prop_node
		if ( measure_props = TryFindFactoryProps( GetMeasureFactory(), props, "Measure" ) )
			model_->CreateMeasure( measure_props, info_ );

		// update the minimize flag in objective_info
		if ( model_->GetMeasure() )
			info_.set_minimize( model_->GetMeasure()->GetMinimize() );

		signature_ = model_->GetSignature();

		AddExternalResources( *model_ );
	}

	result<fitness_t> ModelObjective::evaluate( const SearchPoint& point, const xo::stop_token& st ) const
	{
		std::vector< result< fitness_t > > results;
		for ( int evaluations = 0; evaluations < num_evaluations_to_report;
			  evaluations++ )
		{
			if ( ! st.stop_requested() )
			{
				SearchPoint params( point );
				auto model = CreateModelFromParams( params );
				results.push_back( EvaluateModel( *model, st ) );
			}
			else
				return xo::error_message( "Optimization canceled" );
		}
		
		// Once the simulations are evaluated, return the max if we minimize or
		// the min if we maximize (always the worst case).
		if ( info().minimize() )
			return *std::max_element( results.begin(), results.end(),
									  [] ( const auto& r1, const auto& r2 ) {
										  return r1.value() < r2.value();
									  } );
		else
			return *std::min_element( results.begin(), results.end(),
									  [] ( const auto& r1, const auto& r2 ) {
										  return r1.value() < r2.value();
									  } );
	}

	result<fitness_t> ModelObjective::EvaluateModel( Model& m, const xo::stop_token& st ) const
	{
		m.SetSimulationEndTime( GetDuration() );
		for ( TimeInSeconds t = evaluation_step_size_; !m.HasSimulationEnded(); t += evaluation_step_size_ )
		{
			if ( st.stop_requested() )
				return xo::error_message( "Optimization canceled" );
			AdvanceSimulationTo( m, t );
		}
		return GetResult( m );
	}

	ModelUP ModelObjective::CreateModelFromParams( Params& par ) const
	{
		auto model = CreateModel( model_props, par, GetExternalResourceDir() );
		model->SetSimulationEndTime( GetDuration() );

		if ( controller_props ) // A controller was defined OUTSIDE the model prop_node
			model->CreateController( controller_props, par );

		if ( measure_props ) // A measure was defined OUTSIDE the model prop_node
			model->CreateMeasure( measure_props, par );

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

	ModelObjectiveUP CreateModelObjective( const PropNode& scenario_pn, const path& dir )
	{
		// find objective
		FactoryProps opt_props = FindFactoryProps( GetOptimizerFactory(), scenario_pn, "Optimizer" );
		FactoryProps obj_props = FindFactoryProps( GetObjectiveFactory(), opt_props.props(), "Objective" );

		// create ModelObjective object
		auto mob = dynamic_unique_cast<ModelObjective>( CreateObjective( obj_props, dir ) );

		// read mean / std from init file
		if ( opt_props.props().has_key( "init_file" ) && opt_props.props().get< bool >( "use_init_file", true ) )
		{
			auto init_file = opt_props.props().get< path >( "init_file" );
			auto result = mob->info().import_mean_std( init_file, opt_props.props().get< bool >( "use_init_file_std", true ) );
			log::debug( "Imported ", result.first, " of ", mob->dim(), ", skipped ", result.second, " parameters from ", init_file );
		}

		// report unused properties
		LogUnusedProperties( obj_props.props() );

		return mob;
	}
}

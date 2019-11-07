/*
** StudioModel.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "StudioModel.h"

#include "scone/optimization/opt_tools.h"
#include "scone/core/StorageIo.h"
#include "scone/core/profiler_config.h"
#include "scone/core/Factories.h"
#include "scone/core/system_tools.h"
#include "scone/core/Settings.h"
#include "scone/core/math.h"
#include "scone/model/Muscle.h"

#include "xo/time/timer.h"
#include "xo/filesystem/filesystem.h"
#include "xo/utility/color.h"
#include "xo/geometry/path_alg.h"
#include "xo/geometry/quat.h"
#include "xo/shape/sphere.h"

#include "StudioSettings.h"
#include "QMessageBox"
#include "qt_convert.h"

namespace scone
{
	StudioModel::StudioModel( vis::scene& s, const path& file ) :
		is_evaluating( false )
	{
		// create the objective from par file or config file
		filename_ = file;
		scenario_filename_ = FindScenario( file );
		scenario_pn_ = xo::load_file_with_include( FindScenario( file ), "INCLUDE" );
		optimizer_ = CreateOptimizer( scenario_pn_, file.parent_path() );
		model_objective_ = dynamic_cast<ModelObjective*>( &optimizer_->GetObjective() );

		// create model from par or with default parameters
		const auto file_type = file.extension_no_dot();
		if ( file_type == "par" )
			model_ = GetModelObjective().CreateModelFromParFile( file );
		else // #todo: use ModelObjective::model_ instead? Needs proper parameter initialization
			model_ = GetModelObjective().CreateModelFromParams( SearchPoint( GetModelObjective().info() ) );

		if ( file_type == "sto" )
		{
			// file is a .sto, load results
			xo::timer t;
			log::debug( "Reading ", file );
			ReadStorageSto( storage_, file );
			InitStateDataIndices();
			log::trace( "Read ", file, " in ", t(), " seconds" );
		} else {
			// file is a .par or .scone, setup for evaluation
			is_evaluating = true;
			model_->SetStoreData( true );
			EvaluateTo( 0 ); // evaluate one step so we can init vis
		}

		log::info( "Loaded ", file.filename(), "; dim=", model_objective_->dim() );

		// create and init visualizer
		vis_ = std::make_unique<ModelVis>( *model_, s );
		UpdateVis( 0 );
	}

	StudioModel::~StudioModel()
	{}

	void StudioModel::InitStateDataIndices()
	{
		// setup state_data_index (lazy init)
		SCONE_ASSERT( state_data_index.empty() );
		model_state = model_->GetState();
		state_data_index.resize( model_state.GetSize() );
		for ( size_t state_idx = 0; state_idx < state_data_index.size(); state_idx++ )
		{
			auto data_idx = ( storage_.GetChannelIndex( model_state.GetName( state_idx ) ) );
			SCONE_ASSERT_MSG( data_idx != NoIndex, "Could not find state channel " + model_state.GetName( state_idx ) );
			state_data_index[ state_idx ] = data_idx;
		}
	}

	void StudioModel::UpdateVis( TimeInSeconds time )
	{
		SCONE_PROFILE_FUNCTION;

		if ( !is_evaluating )
		{
			// update model state from data
			SCONE_ASSERT( !state_data_index.empty() );
			for ( index_t i = 0; i < model_state.GetSize(); ++i )
				model_state[ i ] = storage_.GetInterpolatedValue( time, state_data_index[ i ] );
			model_->SetState( model_state, time );
		}

		vis_->Update( *model_ );
	}

	void StudioModel::EvaluateTo( TimeInSeconds t )
	{
		SCONE_ASSERT( IsEvaluating() );
		try
		{
			GetModelObjective().AdvanceSimulationTo( *model_, t );
			if ( model_->HasSimulationEnded() )
				FinalizeEvaluation( true );
		}
		catch ( std::exception& e )
		{
			FinalizeEvaluation( false );
			QString title = "Error evaluating " + to_qt( filename_.filename() );
			QString msg = e.what();
			log::error( title.toStdString(), msg.toStdString() );
			QMessageBox::critical( nullptr, title, msg );
		}
	}

	void StudioModel::FinalizeEvaluation( bool output_results )
	{
		// copy data and init data
		storage_ = model_->GetData();
		if ( !storage_.IsEmpty() )
			InitStateDataIndices();

		if ( output_results )
		{
			auto fitness = model_objective_->GetResult( *model_ );
			log::info( "fitness = ", fitness );
			PropNode results;
			results.push_back( "result", model_objective_->GetReport( *model_ ) );
			auto result_files = model_->WriteResults( filename_ );

			log::debug( "Results written to ", path( filename_ ).replace_extension( "sto" ) );
			log::info( results );
		}

		is_evaluating = false;
	}

	void StudioModel::ApplyViewSettings( const ModelVis::ViewSettings& flags )
	{
		vis_->ApplyViewSettings( flags );
		vis_->Update( *model_ );
	}
}

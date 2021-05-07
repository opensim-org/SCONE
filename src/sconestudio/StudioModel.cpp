/*
** StudioModel.cpp
**
** Copyright (C) 2013-2021 Thomas Geijtenbeek and contributors. All rights reserved.
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
#include "xo/serialization/serialize.h"
#include "xo/shape/shape_tools.h"

#include "StudioSettings.h"

#include <QMessageBox>
#include "qt_convert.h"

namespace scone
{
	StudioModel::StudioModel( vis::scene& s, const path& file ) :
		status_( Status::Initializing )
	{
		// create the objective from par file or config file
		xo::timer load_time;
		filename_ = file;
		scenario_filename_ = FindScenario( file );
		scenario_pn_ = xo::load_file_with_include( FindScenario( file ), "INCLUDE" );
		optimizer_ = CreateOptimizer( scenario_pn_, file.parent_path() );
		objective_ = &optimizer_->GetObjective();
		model_objective_ = dynamic_cast<ModelObjective*>( objective_ );

		if ( model_objective_ )
		{
			try
			{
				// create model from par or with default parameters
				const auto file_type = file.extension_no_dot();
				if ( file_type == "par" )
				{
					model_objective_->info().import_mean_std( file, true );
					model_ = model_objective_->CreateModelFromParFile( file );
				}
				else  // #todo: use ModelObjective::model_ instead? Needs proper parameter initialization
				{
					auto par = SearchPoint( model_objective_->info() );
					model_ = model_objective_->CreateModelFromParams( par );
				}

				if ( file_type == "sto" )
				{
					// file is a .sto, load results
					xo::timer t;
					log::debug( "Reading ", file );
					ReadStorageSto( storage_, file );
					InitStateDataIndices();
					log::trace( "Read ", file, " in ", t(), " seconds" );
					status_ = Status::Ready;
				}
				else
				{
					// file is a .par or .scone, setup for evaluation
					status_ = Status::Evaluating;
					model_->SetStoreData( true );
					EvaluateTo( 0 ); // evaluate one step so we can init vis
				}

				// create and init visualizer
				vis_ = std::make_unique<ModelVis>( *model_, s );
				UpdateVis( 0 );
			}
			catch ( const std::exception& e )
			{
				InvokeError( e.what() );
			}
		}
		else
		{
			log::warning( "Not a model objective, disabling visualization" );
		}

		log::info( "Loaded ", file.filename(), "; dim=", objective_->dim(), "; time=", load_time() );
	}

	StudioModel::~StudioModel()
	{}

	void StudioModel::InitStateDataIndices()
	{
		SCONE_ASSERT( model_ );
		SCONE_ASSERT( state_data_index.empty() );
		SCONE_ERROR_IF( storage_.IsEmpty(), "Could not find any data" );
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
		if ( model_ && vis_ )
		{
			SCONE_PROFILE_FUNCTION( model_->GetProfiler() );
			try
			{
				if ( !storage_.IsEmpty() && !state_data_index.empty() )
				{
					// update model state from data
					for ( index_t i = 0; i < model_state.GetSize(); ++i )
						model_state[ i ] = storage_.GetInterpolatedValue( time, state_data_index[ i ] );
					model_->SetState( model_state, time );
				}

				vis_->Update( *model_ );
			}
			catch ( std::exception& e )
			{
				InvokeError( e.what() );
			}
		}
	}

	void StudioModel::EvaluateTo( TimeInSeconds t )
	{
		if ( model_ && IsEvaluating() )
		{
			try
			{
				model_objective_->AdvanceSimulationTo( *model_, t );
				if ( model_->HasSimulationEnded() )
					FinalizeEvaluation();
			}
			catch ( std::exception& e )
			{
				InvokeError( e.what() );
			}
		}
		else log::warning( "Unexpected call to StudioModel::EvaluateTo()" );
	}

	void StudioModel::AbortEvaluation()
	{
		try
		{
			status_ = Status::Aborted;
			storage_ = model_->GetData();
			InitStateDataIndices();
		}
		catch ( const std::exception& e )
		{
			InvokeError( e.what() );
		}
	}

	PropNode StudioModel::GetResult() const
	{
		PropNode report;
		report.add_child( "Result", model_objective_->GetReport( *model_ ) );
		if ( auto simpn = model_->GetSimulationReport(); !simpn.empty() )
			report.add_child( "Simulation", model_->GetSimulationReport() );
		return report;
	}

	void StudioModel::FinalizeEvaluation()
	{
		if ( model_objective_ )
		{
			try
			{
				// fetch data
				storage_ = model_->GetData();
				InitStateDataIndices();

				// show fitness results
				auto fitness = model_objective_->GetResult( *model_ );
				//log::info( "fitness = ", fitness );
				PropNode results = GetResult();
				log::info( results );

				// write results to file(s)
				xo::timer t;
				auto result_files = model_->WriteResults( filename_ );
				log::debug( "Results written to ", concatenate_str( result_files, ", " ), " in ", t().secondsd(), "s" );

				// we're done!
				status_ = Status::Ready;
			}
			catch ( std::exception& e )
			{
				InvokeError( e.what() );
			}
		}
		else log::warning( "Unexpected call to StudioModel::FinalizeEvaluation()" );
	}

	void StudioModel::InvokeError( const String& message )
	{
		if ( status_ != Status::Error )
		{
			status_ = Status::Error;
			log::error( "Error in ", filename_.filename(), ": ", message );
			QMessageBox::critical( nullptr, "Error in " + to_qt( filename_.filename() ), message.c_str() );
		}
		else log::error( message );
	}

	TimeInSeconds StudioModel::GetMaxTime() const
	{
		if ( model_objective_ && IsEvaluating() )
			return model_objective_->GetDuration();
		else if ( !storage_.IsEmpty() )
			return storage_.Back().GetTime();
		else return 0.0;
	}

	void StudioModel::ApplyViewSettings( const ModelVis::ViewSettings& flags )
	{
		if ( vis_ )
		{
			vis_->ApplyViewSettings( flags );
			vis_->Update( *model_ );
		}
	}

	const ModelVis::ViewSettings& StudioModel::GetViewSettings() const
	{
		SCONE_ASSERT( vis_ );
		return vis_->GetViewSettings();
	}

	Vec3 StudioModel::GetFollowPoint() const
	{
		auto com = model_->GetComPos();
		if ( auto gp = model_->GetGroundPlane() )
		{
			auto l = xo::linef( xo::vec3f( com ), xo::vec3f::neg_unit_y() );
			auto& p = std::get<xo::plane>( gp->GetShape() );
			auto t = xo::transformf( xo::vec3f( gp->GetPos() ), xo::quatf( gp->GetOri() ) );
			com.y = xo::intersection( l, p, t ).y;
		}
		else com.y = 0;
		return com;
	}
}

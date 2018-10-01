/*
** StudioModel.cpp
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "StudioModel.h"

#include "scone/optimization/opt_tools.h"
#include "scone/core/StorageIo.h"
#include "scone/core/Profiler.h"
#include "scone/core/Factories.h"
#include "scone/core/system_tools.h"
#include "scone/core/Settings.h"
#include "scone/core/math.h"
#include "scone/model/Muscle.h"

#include "xo/time/timer.h"
#include "xo/filesystem/filesystem.h"
#include "simvis/color.h"
#include "xo/geometry/path_alg.h"
#include "StudioSettings.h"

namespace scone
{
	StudioModel::StudioModel( vis::scene& s, const path& file, bool force_evaluation ) :
	bone_mat( GetStudioSetting< vis::color >( "viewer.bone" ), 1, 15, 0, 0.5f ),
	muscle_mat( vis::make_blue(), 0.5f, 15, 0, 0.5f ),
	tendon_mat( GetStudioSetting< vis::color >( "viewer.tendon" ), 0.5f, 15, 0, 0.5f ),
	arrow_mat( GetStudioSetting< vis::color >( "viewer.force" ), 1.0, 15, 0, 0.5f ),
	is_evaluating( false )
	{
		view_flags.set( ShowForces ).set( ShowMuscles ).set( ShowGeometry ).set( EnableShadows );

		// create the objective form par file or config file
		model_objective = CreateModelObjective( file );
		SearchPoint par( model_objective->info() );
		if ( file.extension() == "par" )
			par.import_values( file );
		model = model_objective->CreateModelFromParams( par );

		// accept filename and clear data
		filename = file;

		// see if we can load a matching .sto file
		auto sto_file = xo::try_find_file( { file + ".sto", path( file ).replace_extension( ".sto" ) } );
		if ( !force_evaluation && sto_file && filename.extension() == "par" )
		{
			xo::timer t;
			log::info( "Reading ", *sto_file );
			ReadStorageSto( data, *sto_file );
			InitStateDataIndices();
			log::trace( "Read ", sto_file, " in ", t.seconds(), " seconds" );
		}
		else
		{
			// start evaluation
			is_evaluating = true;
			model->SetStoreData( true, 1.0 / GetSconeSettings().get< double >( "data.frequency" ) );
			if ( GetSconeSettings().get< bool >( "data.muscle" ) )
				model->GetStoreDataFlags().set( { StoreDataTypes::MuscleExcitation, StoreDataTypes::MuscleFiberProperties } );
			if ( GetSconeSettings().get< bool >( "data.body" ) )
				model->GetStoreDataFlags().set( { StoreDataTypes::BodyComPosition, StoreDataTypes::BodyOrientation } );
			if ( GetSconeSettings().get< bool >( "data.sensor" ) )
				model->GetStoreDataFlags().set( { StoreDataTypes::SensorData } );
			if ( GetSconeSettings().get< bool >( "data.controller" ) )
				model->GetStoreDataFlags().set( { StoreDataTypes::ControllerData } );

			model->SetSimulationEndTime( model_objective->GetDuration() );
			log::info( "Evaluating ", filename );
			EvaluateTo( 0 ); // evaluate one step so we can init vis
		}

		InitVis( s );
		UpdateVis( 0 );
	}

	StudioModel::~StudioModel()
	{}

	void StudioModel::InitStateDataIndices()
	{
		// setup state_data_index (lazy init)
		SCONE_ASSERT( state_data_index.empty() );
		model_state = model->GetState();
		state_data_index.resize( model_state.GetSize() );
		for ( size_t state_idx = 0; state_idx < state_data_index.size(); state_idx++ )
		{
			auto data_idx = data.GetChannelIndex( model_state.GetName( state_idx ) );
			SCONE_ASSERT_MSG( data_idx != NoIndex, "Could not find state channel " + model_state.GetName( state_idx ) );
			state_data_index[ state_idx ] = data_idx;
		}
	}

	void StudioModel::InitVis( vis::scene& scone_scene )
	{
		scone_scene.attach( root );

		xo::timer t;
		for ( auto& body : model->GetBodies() )
		{
			bodies.push_back( root.add_group() );
			body_axes.push_back( bodies.back().add_axes( vis::vec3f( 0.1, 0.1, 0.1 ), 0.5f ) );

			auto geom_files = body->GetDisplayGeomFileNames();
			for ( auto geom_file : geom_files )
			{
				//log::trace( "Loading geometry for body ", body->GetName(), ": ", geom_file );
				try
				{
					if ( !xo::file_exists( geom_file ) )
						geom_file = scone::GetFolder( scone::SCONE_GEOMETRY_FOLDER ) / geom_file;

					body_meshes.push_back( bodies.back().add_mesh( geom_file ) );
					body_meshes.back().set_material( bone_mat );
				}
				catch ( std::exception& e )
				{
					log::warning( "Could not load ", geom_file, ": ", e.what() );
				}
			}
		}
		log::debug( "Meshes loaded in ", t.seconds(), " seconds" );

		for ( auto& cg : model->GetContactGeometries() )
		{
			auto idx = FindIndexByName( model->GetBodies(), cg.m_Body.GetName() );
			auto& parent = idx != NoIndex ? bodies[ idx ] : root;
			contact_geoms.push_back( parent.add_sphere( cg.m_Scale.x, GetStudioSetting< vis::color >( "viewer.contact" ), 0.75f ) );
			contact_geoms.back().pos( cg.m_Pos );
		}

		for ( auto& muscle : model->GetMuscles() )
		{
			// add path
			MuscleVis mv;
			mv.ten1 = root.add< vis::trail >( 1, 0.005f, vis::make_yellow(), 0.3f );
			mv.ten2 = root.add< vis::trail >( 1, 0.005f, vis::make_yellow(), 0.3f );
			mv.ten1.set_material( tendon_mat );
			mv.ten2.set_material( tendon_mat );

			mv.ce = root.add< vis::trail >( 1, 0.0075f, vis::make_red(), 0.5f );
			mv.mat = muscle_mat.clone();
			mv.ce.set_material( mv.mat );
			muscles.push_back( mv );
		}

		ApplyViewSettings( view_flags );
	}

	void StudioModel::UpdateVis( TimeInSeconds time )
	{
		SCONE_PROFILE_FUNCTION;

		index_t force_count = 0;

		// initialize visualization
		std::unique_lock< std::mutex > simulation_lock( model->GetSimulationMutex(), std::defer_lock );

		if ( !is_evaluating )
		{
			// update model state from data
			SCONE_ASSERT( !state_data_index.empty() );
			for ( index_t i = 0; i < model_state.GetSize(); ++i )
				model_state[ i ] = data.GetInterpolatedValue( time, state_data_index[ i ] );
			model->SetState( model_state, time );
		}

		// update com
		//com.pos( model->GetComPos() );

		// update bodies
		auto& model_bodies = model->GetBodies();
		for ( index_t i = 0; i < model_bodies.size(); ++i )
		{
			auto& b = model_bodies[ i ];
			vis::transformf trans( b->GetOriginPos(), b->GetOrientation() );
			bodies[ i ].transform( trans );

			// external forces / moments
			auto f = b->GetExternalForce();
			if ( !f.is_null() )
				UpdateForceVis( force_count++, b->GetPosOfPointOnBody( b->GetExternalForcePoint() ), f );
			auto m = b->GetExternalMoment();
			if ( !m.is_null() )
				UpdateForceVis( force_count++, b->GetComPos(), m );
		}

		// update muscle paths
		auto &model_muscles = model->GetMuscles();
		for ( index_t i = 0; i < model_muscles.size(); ++i )
			UpdateMuscleVis( *model_muscles[ i ], muscles[ i ] );

		// update ground reaction forces on legs
		for ( index_t i = 0; i < model->GetLegCount(); ++i )
		{
			Vec3 force, moment, cop;
			model->GetLeg( i ).GetContactForceMomentCop( force, moment, cop );

			if ( force.squared_length() > REAL_WIDE_EPSILON && view_flags.get< ShowForces >() )
				UpdateForceVis( force_count++, cop, force );
		}

		if ( force_count < forces.size() )
			forces.resize( force_count );
	}

	void StudioModel::UpdateForceVis( index_t force_idx, Vec3 cop, Vec3 force )
	{
		while ( forces.size() <= force_idx )
		{
			forces.push_back( root.add_arrow( 0.01f, 0.02f, vis::make_yellow(), 0.3f ) );
			forces.back().set_material( arrow_mat );
			forces.back().show( view_flags.get< ShowForces >() );
		}
		forces[ force_idx ].pos( cop, cop + 0.001 * force );
	}

	void StudioModel::UpdateMuscleVis( const class Muscle& mus, MuscleVis& vis )
	{
		auto mp = mus.GetMusclePath();
		auto len = mus.GetLength();
		auto tlen = mus.GetTendonLength() / 2;
		auto a = mus.GetActivation();
		auto p = mus.GetMusclePath();

		auto i1 = insert_path_point( p, tlen );
		auto i2 = insert_path_point( p, len - tlen );

		vis.mat.diffuse( vis::color( a, 0, 0.5 - 0.5 * a, 1 ) );
		vis.mat.emissive( vis::color( a, 0, 0.5 - 0.5 * a, 1 ) );
		vis.ten1.set_points( p.begin(), p.begin() + i1 + 1 );
		vis.ce.set_points( p.begin() + i1, p.begin() + i2 + 1 );
		vis.ten2.set_points( p.begin() + i2, p.end() );
	}

	void StudioModel::EvaluateTo( TimeInSeconds t )
	{
		SCONE_ASSERT( IsEvaluating() );
		model_objective->AdvanceModel( *model, t );
		if ( model->GetTerminationRequest() || t >= model->GetSimulationEndTime() )
			FinalizeEvaluation( true );
	}

	void StudioModel::FinalizeEvaluation( bool output_results )
	{
		auto rate = ( model->GetData().GetFrameCount() - 1 ) / model->GetData().Back().GetTime();
		auto target = GetSconeSettings().get< double >( "data.frequency" );
		auto stride = xo::max( 1, int( std::round( rate / target ) ) );
		//log::debug( "Downsampling from ", rate, "Hz to ", target, "Hz; stride = ", stride );

		// copy data and init data
		data = model->GetData();
		if ( !data.IsEmpty() )
			InitStateDataIndices();

		if ( output_results )
		{
			auto fitness = model_objective->GetResult( *model );
			log::info( "fitness = ", fitness );
			PropNode results;
			results.push_back( "result", model_objective->GetReport( *model ) );
			model->WriteResults( filename );

			log::info( "Results written to ", path( filename ).replace_extension( "sto" ) );
			log::info( results );
		}

		// reset this stuff
		is_evaluating = false;
	}

	void StudioModel::ApplyViewSettings( const ViewFlags& flags )
	{
		view_flags = flags;
		for ( auto& f : forces ) f.show( view_flags.get< ShowForces >() );
		for ( auto& m : muscles ) m.ce.show( view_flags.get< ShowMuscles >() );
		for ( auto& e : body_meshes ) e.show( view_flags.get< ShowGeometry >() ); // for ( auto m : e ) m.show( view_flags.get< ShowGeometry >() );
		for ( auto& e : body_axes ) e.show( view_flags.get< ShowAxes >() );
		for ( auto& e : contact_geoms ) e.show( view_flags.get< ShowContactGeom >() );
	}
}

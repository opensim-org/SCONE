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
#include "xo/geometry/euler_angles.h"

#include "xo/time/timer.h"
#include "xo/filesystem/filesystem.h"
#include "xo/utility/color.h"
#include "xo/geometry/path_alg.h"
#include "StudioSettings.h"
#include "xo/shape/sphere.h"
#include "QMessageBox"
#include "qt_convert.h"

namespace scone
{
	StudioModel::StudioModel( vis::scene& s, const path& file, bool force_evaluation ) :
	scene_( s ),
	root( &scene_ ),
	specular_( GetStudioSetting < float > ( "viewer.specular" ) ),
	shininess_( GetStudioSetting< float >( "viewer.shininess" ) ),
	ambient_( GetStudioSetting< float >( "viewer.ambient" ) ),
	emissive_( GetStudioSetting< float >( "viewer.emissive" ) ),
	bone_mat( GetStudioSetting< xo::color >( "viewer.bone" ), specular_, shininess_, ambient_, emissive_ ),
	muscle_mat( GetStudioSetting< xo::color >( "viewer.muscle_0" ), specular_, shininess_, ambient_, emissive_ ),
	tendon_mat( GetStudioSetting< xo::color >( "viewer.tendon" ), specular_, shininess_, ambient_, emissive_ ),
	arrow_mat( GetStudioSetting< xo::color >( "viewer.force" ), specular_, shininess_, ambient_, emissive_ ),
	contact_mat( GetStudioSetting< xo::color >( "viewer.contact" ), specular_, shininess_, ambient_, emissive_ ),
	muscle_gradient( { { 0.0f, GetStudioSetting< xo::color >( "viewer.muscle_0" ) }, { 0.5f, GetStudioSetting< xo::color >( "viewer.muscle_50" ) }, { 1.0f, GetStudioSetting< xo::color >( "viewer.muscle_100" ) } } ),
	is_evaluating( false )
	{
		// #todo: don't reset this every time, perhaps keep view_flags outside StudioModel
		view_flags.set( { ShowForces, ShowMuscles, ShowTendons, ShowGeometry, EnableShadows } );

		// create the objective from par file or config file
		scenario_pn_ = xo::load_file_with_include( FindScenario( file ), "INCLUDE" );
		model_objective = CreateModelObjective( scenario_pn_, file.parent_path() );
		log::info( "Created objective ", model_objective->GetSignature(), "; dim=", model_objective->dim(), " source=", file.filename() );

		// create model from par or with default parameters
		if ( file.extension_no_dot() == "par" )
			model_ = model_objective->CreateModelFromParFile( file );
		else
			model_ = model_objective->CreateModelFromParams( SearchPoint( model_objective->info() ) );

		// accept filename and clear data
		filename_ = file;

		// load results if the file is an sto
		if ( file.extension_no_dot() == "sto" && !force_evaluation )
		{
			xo::timer t;
			log::debug( "Reading ", file );
			ReadStorageSto( data, file );
			InitStateDataIndices();
			log::trace( "Read ", file, " in ", t(), " seconds" );
		}
		else
		{
			// start evaluation
			is_evaluating = true;
			model_->SetStoreData( true );
			log::debug( "Evaluating ", filename_ );
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
		model_state = model_->GetState();
		state_data_index.resize( model_state.GetSize() );
		for ( size_t state_idx = 0; state_idx < state_data_index.size(); state_idx++ )
		{
			auto data_idx = (data.GetChannelIndex( model_state.GetName( state_idx ) ));
			SCONE_ASSERT_MSG( data_idx != NoIndex, "Could not find state channel " + model_state.GetName( state_idx ) );
			state_data_index[ state_idx ] = data_idx;
		}
	}

	void StudioModel::InitVis( vis::scene& scone_scene )
	{
		scone_scene.attach( root );

		// ground plane
		if ( auto* gp = model_->GetGroundPlane() )
		{
			ground_ = vis::plane( root, 128, 128, 0.5f, scone::GetStudioSetting< xo::color >( "viewer.tile1" ), scone::GetStudioSetting< xo::color >( "viewer.tile2" ) );
			//ground_plane = scene_.add< vis::plane >( xo::vec3f( 64, 0, 0 ), xo::vec3f( 0, 0, -64 ), GetFolder( SCONE_UI_RESOURCE_FOLDER ) / "stile160.png", 64, 64 );
			ground_.pos_ori( gp->GetPos(), xo::quat_from_z_angle( 90_deg ) * gp->GetOri() );
		}

		xo::timer t;
		for ( auto& body : model_->GetBodies() )
		{
			bodies.push_back( vis::node( &root ) );
			body_axes.push_back( vis::axes( bodies.back(), vis::vec3f( 0.1, 0.1, 0.1 ), 0.5f ) );

			auto geoms = body->GetDisplayGeometries();
			for ( auto geom : geoms )
			{
				//log::trace( "Loading geometry for body ", body->GetName(), ": ", geom_file );
				try
				{
					auto geom_file = xo::try_find_file( { geom.filename, path( "./geometry" ) / geom.filename, scone::GetFolder( scone::SCONE_GEOMETRY_FOLDER ) / geom.filename } );
					if ( geom_file )
					{
						body_meshes.push_back( vis::mesh( bodies.back(), *geom_file ) );
						body_meshes.back().set_material( bone_mat );
						body_meshes.back().pos_ori( geom.pos, geom.ori );
						body_meshes.back().scale( geom.scale );
					}
					else log::warning( "Could not find ", geom.filename );
				}
				catch ( std::exception& e )
				{
					log::warning( "Could not load ", geom.filename, ": ", e.what() );
				}
			}
		}
		//log::debug( "Meshes loaded in ", t.seconds(), " seconds" );

		for ( auto& cg : model_->GetContactGeometries() )
		{
			auto idx = FindIndexByName( model_->GetBodies(), cg->GetBody().GetName() );
			auto& parent = idx != NoIndex ? bodies[ idx ] : root;
			if ( std::holds_alternative<xo::sphere>( cg->GetShape() ) )
			{
				// #todo: add support for other shapes (i.e. planes)
				contact_geoms.push_back( vis::mesh( parent, cg->GetShape(), xo::color::cyan(), xo::vec3f::zero(), 0.75f ) );
				contact_geoms.back().set_material( contact_mat );
				contact_geoms.back().pos( cg->GetPos() );
			}
		}

		for ( auto& muscle : model_->GetMuscles() )
		{
			auto muscle_radius = GetStudioSetting<bool>( "viewer.auto_muscle_width" ) ?
				GetStudioSetting<float>( "viewer.auto_muscle_width_factor" ) * sqrt( muscle->GetPCSA() / xo::constantsd::pi() ) :
				GetStudioSetting<float>( "viewer.muscle_width" );

			auto tendon_radius = GetStudioSetting<float>( "viewer.relative_tendon_width" ) * muscle_radius;

			// add path
			MuscleVis mv;
			mv.ten1 = vis::trail( root, 1, tendon_radius, xo::color::yellow(), 0.3f );
			mv.ten2 = vis::trail( root, 1, tendon_radius, xo::color::yellow(), 0.3f );
			mv.ten1.set_material( tendon_mat );
			mv.ten2.set_material( tendon_mat );
			mv.ce = vis::trail( root, 1, muscle_radius, xo::color::red(), 0.5f );
			mv.mat = muscle_mat.clone();
			mv.ce.set_material( mv.mat );
			mv.ce_pos = GetStudioSetting<float>( "viewer.muscle_position" );
			muscles.push_back( std::move( mv ) );
		}

		ApplyViewSettings( view_flags );
	}

	void StudioModel::UpdateVis( TimeInSeconds time )
	{
		SCONE_PROFILE_FUNCTION;

		index_t force_count = 0;

		if ( !is_evaluating )
		{
			// update model state from data
			SCONE_ASSERT( !state_data_index.empty() );
			for ( index_t i = 0; i < model_state.GetSize(); ++i )
				model_state[ i ] = data.GetInterpolatedValue( time, state_data_index[ i ] );
			model_->SetState( model_state, time );
		}

		// update bodies
		auto& model_bodies = model_->GetBodies();
		for ( index_t i = 0; i < model_bodies.size(); ++i )
		{
			auto& b = model_bodies[ i ];
			vis::transformf trans( b->GetOriginPos(), b->GetOrientation() );
			bodies[ i ].transform( trans );

			// external forces
			if ( auto f = b->GetExternalForce(); !f.is_null() )
				UpdateForceVis( force_count++, b->GetPosOfPointOnBody( b->GetExternalForcePoint() ), f );

			// external moments
			if ( auto m = b->GetExternalMoment(); !m.is_null() )
				UpdateForceVis( force_count++, b->GetComPos(), m );
		}

		// update muscle paths
		auto &model_muscles = model_->GetMuscles();
		for ( index_t i = 0; i < model_muscles.size(); ++i )
			UpdateMuscleVis( *model_muscles[ i ], muscles[ i ] );

		// update ground reaction forces on legs
#ifdef USE_GRF
		for ( index_t i = 0; i < model_->GetLegCount(); ++i )
		{
			Vec3 force, moment, cop;
			model_->GetLeg( i ).GetContactForceMomentCop( force, moment, cop );

			if ( force.squared_length() > REAL_WIDE_EPSILON && view_flags.get< ShowForces >() )
				UpdateForceVis( force_count++, cop, force );
		}
#endif // USE_GRF

		for ( auto& cf : model_->GetContactForces() )
		{
			auto&[ force, moment, point ] = cf->GetForceMomentPoint();
			if ( force.squared_length() > REAL_WIDE_EPSILON && view_flags.get< ShowForces >() )
				UpdateForceVis( force_count++, point, force );
		}

		while ( force_count < forces.size() )
			forces.pop_back();
	}

	void StudioModel::UpdateForceVis( index_t force_idx, Vec3 cop, Vec3 force )
	{
		while ( forces.size() <= force_idx )
		{
			forces.emplace_back( root, 0.01f, 0.02f, xo::color::yellow(), 0.3f );
			forces.back().set_material( arrow_mat );
			forces.back().show( view_flags.get< ShowForces >() );
		}
		forces[ force_idx ].pos( cop, cop + 0.001 * force );
	}

	void StudioModel::UpdateMuscleVis( const class Muscle& mus, MuscleVis& vis )
	{
		auto mp = mus.GetMusclePath();
		auto mlen = mus.GetFiberLength();
		auto tlen = std::max( 0.0, mus.GetTendonLength() * vis.ce_pos );
		auto a = mus.GetActivation();
		auto p = mus.GetMusclePath();

		xo::color c = muscle_gradient( float( a ) );
		vis.mat.diffuse( c );
		vis.mat.emissive( c );

		if ( view_flags.get<ShowTendons>() )
		{
			auto i1 = insert_path_point( p, tlen );
			auto i2 = insert_path_point( p, tlen + mlen );
			vis.ten1.set_points( p.begin(), p.begin() + i1 + 1 );
			vis.ce.set_points( p.begin() + i1, p.begin() + i2 + 1 );
			vis.ten2.set_points( p.begin() + i2, p.end() );
		}
		else
		{
			vis.ce.set_points( p.begin(), p.end() );
		}
	}

	void StudioModel::EvaluateTo( TimeInSeconds t )
	{
		SCONE_ASSERT( IsEvaluating() );
		try
		{
			model_objective->AdvanceSimulationTo( *model_, t );
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
		data = model_->GetData();
		if ( !data.IsEmpty() )
			InitStateDataIndices();

		if ( output_results )
		{
			auto fitness = model_objective->GetResult( *model_ );
			log::info( "fitness = ", fitness );
			PropNode results;
			results.push_back( "result", model_objective->GetReport( *model_ ) );
			model_->WriteResults( filename_ );

			log::debug( "Results written to ", path( filename_ ).replace_extension( "sto" ) );
			log::info( results );
		}

		is_evaluating = false;
	}

	void StudioModel::ApplyViewSettings( const ViewFlags& flags )
	{
		view_flags = flags;
		for ( auto& f : forces )
			f.show( view_flags.get< ShowForces >() );

		for ( auto& m : muscles )
		{
			m.ce.show( view_flags.get< ShowMuscles >() );
			m.ten1.show( view_flags.get< ShowMuscles >() && view_flags.get< ShowTendons >() );
			m.ten2.show( view_flags.get< ShowMuscles >() && view_flags.get< ShowTendons >() );
		}

		for ( auto& e : body_meshes )
			e.show( view_flags.get< ShowGeometry >() );

		for ( auto& e : body_axes )
			e.show( view_flags.get< ShowAxes >() );

		for ( auto& e : contact_geoms )
			e.show( view_flags.get< ShowContactGeom >() );

		ground_.show( view_flags.get< StudioModel::ShowGroundPlane >() );

		if ( model_ )
			UpdateVis( model_->GetTime() );
	}
}

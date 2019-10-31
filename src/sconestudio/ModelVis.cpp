#include "ModelVis.h"

#include "StudioSettings.h"
#include "vis/scene.h"
#include "xo/filesystem/filesystem.h"
#include "scone/core/Log.h"
#include "scone/model/Muscle.h"
#include "xo/geometry/path_alg.h"
#include "scone/core/math.h"

namespace scone
{
	ModelVis::ModelVis( const Model& model, vis::scene& s ) :
		root_node_( &s ),
		specular_( GetStudioSetting < float >( "viewer.specular" ) ),
		shininess_( GetStudioSetting< float >( "viewer.shininess" ) ),
		ambient_( GetStudioSetting< float >( "viewer.ambient" ) ),
		bone_mat( { GetStudioSetting< xo::color >( "viewer.bone" ), specular_, shininess_, ambient_ } ),
		muscle_mat( { GetStudioSetting< xo::color >( "viewer.muscle_0" ), specular_, shininess_, ambient_ } ),
		tendon_mat( { GetStudioSetting< xo::color >( "viewer.tendon" ), specular_, shininess_, ambient_ } ),
		arrow_mat( { GetStudioSetting< xo::color >( "viewer.force" ), specular_, shininess_, ambient_ } ),
		contact_mat( { GetStudioSetting< xo::color >( "viewer.contact" ), specular_, shininess_, ambient_ } ),
		muscle_gradient( {
			{ 0.0f, GetStudioSetting< xo::color >( "viewer.muscle_0" ) },
			{ 0.5f, GetStudioSetting< xo::color >( "viewer.muscle_50" ) },
			{ 1.0f, GetStudioSetting< xo::color >( "viewer.muscle_100" ) } } )
	{
		// #todo: don't reset this every time, perhaps keep view_flags outside StudioModel
		view_flags.set( { ShowForces, ShowMuscles, ShowTendons, ShowGeometry, EnableShadows } );

		// ground plane
		if ( auto* gp = model.GetGroundPlane() )
		{
			ground_ = vis::plane( root_node_, 128, 128, 0.5f, scone::GetStudioSetting< xo::color >( "viewer.tile1" ), scone::GetStudioSetting< xo::color >( "viewer.tile2" ) );
			//ground_plane = scene_.add< vis::plane >( xo::vec3f( 64, 0, 0 ), xo::vec3f( 0, 0, -64 ), GetFolder( SCONE_UI_RESOURCE_FOLDER ) / "stile160.png", 64, 64 );
			ground_.pos_ori( vis::vec3f( gp->GetPos() ), xo::quat_from_z_angle( 90_deg ) * gp->GetOri() );
		}

		for ( auto& body : model.GetBodies() )
		{
			bodies.push_back( vis::node( &root_node_ ) );
			body_axes.push_back( vis::axes( bodies.back(), vis::vec3f( 0.1, 0.1, 0.1 ), 0.5f ) );

			auto geoms = body->GetDisplayGeometries();
			for ( auto geom : geoms )
			{
				//log::trace( "Loading geometry for body ", body->GetName(), ": ", geom_file );
				try
				{
					auto geom_file = xo::try_find_file( { geom.filename, path( "geometry" ) / geom.filename, scone::GetFolder( scone::SCONE_GEOMETRY_FOLDER ) / geom.filename } );
					if ( geom_file )
					{
						body_meshes.push_back( vis::mesh( bodies.back(), *geom_file ) );
						body_meshes.back().set_material( bone_mat );
						body_meshes.back().pos_ori( vis::vec3f( geom.pos ), vis::quatf( geom.ori ) );
						body_meshes.back().scale( vis::vec3f( geom.scale ) );
					} else log::warning( "Could not find ", geom.filename );
				}
				catch ( std::exception & e )
				{
					log::warning( "Could not load ", geom.filename, ": ", e.what() );
				}
			}
		}

		for ( auto& cg : model.GetContactGeometries() )
		{
			auto idx = FindIndexByName( model.GetBodies(), cg->GetBody().GetName() );
			auto& parent = idx != NoIndex ? bodies[ idx ] : root_node_;
			if ( std::holds_alternative<xo::sphere>( cg->GetShape() ) )
			{
				// #todo: add support for other shapes (i.e. planes)
				contact_geoms.push_back( vis::mesh( parent, cg->GetShape(), xo::color::cyan(), xo::vec3f::zero(), 0.75f ) );
				contact_geoms.back().set_material( contact_mat );
				contact_geoms.back().pos( vis::vec3f( cg->GetPos() ) );
			}
		}

		for ( auto& muscle : model.GetMuscles() )
		{
			auto muscle_radius = GetStudioSetting<bool>( "viewer.auto_muscle_width" ) ?
				GetStudioSetting<float>( "viewer.auto_muscle_width_factor" ) * sqrt( muscle->GetPCSA() / xo::constantsd::pi() ) :
				GetStudioSetting<float>( "viewer.muscle_width" );

			auto tendon_radius = GetStudioSetting<float>( "viewer.relative_tendon_width" ) * muscle_radius;

			// add path
			MuscleVis mv;
			mv.ten1 = vis::trail( root_node_, 1, tendon_radius, xo::color::yellow(), 0.3f );
			mv.ten2 = vis::trail( root_node_, 1, tendon_radius, xo::color::yellow(), 0.3f );
			mv.ten1.set_material( tendon_mat );
			mv.ten2.set_material( tendon_mat );
			mv.ce = vis::trail( root_node_, 1, muscle_radius, xo::color::red(), 0.5f );
			mv.mat = muscle_mat.clone();
			mv.ce.set_material( mv.mat );
			mv.ce_pos = GetStudioSetting<float>( "viewer.muscle_position" );
			muscles.push_back( std::move( mv ) );
		}

		ApplyViewSettings( view_flags );
	}

	ModelVis::~ModelVis()
	{}

	void ModelVis::Update( const Model& model )
	{
		index_t force_count = 0;

		// update bodies
		auto& model_bodies = model.GetBodies();
		for ( index_t i = 0; i < model_bodies.size(); ++i )
		{
			auto& b = model_bodies[ i ];
			vis::transformf trans( vis::vec3f( b->GetOriginPos() ), b->GetOrientation() );
			bodies[ i ].transform( trans );

			// external forces
			if ( auto f = b->GetExternalForce(); !f.is_null() )
				UpdateForceVis( force_count++, b->GetPosOfPointOnBody( b->GetExternalForcePoint() ), f );

			// external moments
			if ( auto m = b->GetExternalMoment(); !m.is_null() )
				UpdateForceVis( force_count++, b->GetComPos(), m );
		}

		// update muscle paths
		auto& model_muscles = model.GetMuscles();
		for ( index_t i = 0; i < model_muscles.size(); ++i )
			UpdateMuscleVis( *model_muscles[ i ], muscles[ i ] );

		// update forces
		for ( auto& cf : model.GetContactForces() )
		{
			auto& [force, moment, point] = cf->GetForceMomentPoint();
			if ( xo::squared_length( force ) > REAL_WIDE_EPSILON && view_flags.get< ShowForces >() )
				UpdateForceVis( force_count++, point, force );
		}
		while ( force_count < forces.size() )
			forces.pop_back();
	}

	void ModelVis::UpdateForceVis( index_t force_idx, Vec3 cop, Vec3 force )
	{
		while ( forces.size() <= force_idx )
		{
			forces.emplace_back( root_node_, 0.01f, 0.02f, xo::color::yellow(), 0.3f );
			forces.back().set_material( arrow_mat );
			forces.back().show( view_flags.get< ShowForces >() );
		}
		forces[ force_idx ].pos( vis::vec3f( cop ), vis::vec3f( cop + 0.001 * force ) );
	}

	void ModelVis::UpdateMuscleVis( const class Muscle& mus, MuscleVis& vis )
	{
		auto mlen = mus.GetFiberLength();
		auto tlen = std::max( 0.0, mus.GetTendonLength() * vis.ce_pos );
		auto a = mus.GetActivation();
		auto p = mus.GetMusclePath();

		xo::color c = muscle_gradient( float( a ) );
		vis.mat.diffuse( c );
		vis.mat.emissive( vis::color() );
		vis.mat.ambient( c );

		if ( view_flags.get<ShowTendons>() )
		{
			auto i1 = insert_path_point( p, tlen );
			auto i2 = insert_path_point( p, tlen + mlen );
			vis.ten1.set_points( p.begin(), p.begin() + i1 + 1 );
			vis.ce.set_points( p.begin() + i1, p.begin() + i2 + 1 );
			vis.ten2.set_points( p.begin() + i2, p.end() );
		} else vis.ce.set_points( p.begin(), p.end() );
	}

	void ModelVis::ApplyViewSettings( const ViewSettings& f )
	{
		view_flags = f;
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

		if ( ground_.node_id() )
			ground_.show( view_flags.get< ShowGroundPlane >() );
	}
}

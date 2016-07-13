#include "StudioScene.h"
#include "scone/core/system_tools.h"
#include "flut/math/math.hpp"

using namespace vis;

namespace scone
{
	StudioScene::StudioScene() :
	scene( true )
	{
		// init scene
		scene.add_light( vec3f( -20, 80, 40 ), make_white( 1 ) );
		//mesh1 = scene.make_mesh( scone::GetFolder( SCONE_GEOMETRY_FOLDER ) + "femur.vtp" );
		//mesh1.pos( vec3f( -0.5, 1, 0 ) );
		//mesh1.set_color( make_blue(), 1, 12, 0.5, 0 );
		//mesh2 = scene.make_sphere( 0.2f, vis::make_red(), 0.9f );
		//mesh2.pos( vec3f( 0.5f, 1, 0 ) );

		//circle = scene.add_path( 21, 0.01f, make_cyan(), 0.9f );
		//std::vector< vec3f > cp( 21 );
		//for ( int i = 0; i < 21; ++ i )
		//	cp[ i ] = vec3f( flut::math::cos( i * flut::math::degree( 18.0 ) ), sin( i * flut::math::real_pi / 10 ) + 1.5, 0 );
		//circle.set_points( cp );
	}

	StudioScene::~StudioScene() { }

	scone::StudioModel& StudioScene::CreateModel( const String& par_file )
	{
		model.reset();
		model = StudioModelUP( new StudioModel( scene, par_file ) );
		return *model;
	}

	void StudioScene::Update( TimeInSeconds t )
	{
		if ( model )
			model->UpdateVis( t );

		//int points = t * 10;
		//std::vector< vec3f > cp( points );
		//for ( int i = 0; i < points; ++ i )
		//	cp[ i ] = vec3f( i * 0.1, sin( i * flut::math::degree( 20.0 ) ) + 1.5, flut::math::cos( i * flut::math::degree( 20.0 ) ) );
		//circle.set_points( cp );
	}

	scone::TimeInSeconds StudioScene::GetMaxTime()
	{
		return model->GetData().IsEmpty() ? 0.0 : model->GetData().Back().GetTime();
	}
}

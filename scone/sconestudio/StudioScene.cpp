#include "StudioScene.h"
#include "scone/core/system_tools.h"

using namespace vis;

namespace scone
{
	StudioScene::StudioScene() :
	scene( false )
	{
		// init scene
		scene.make_light( vec3f( -20, 80, 40 ), make_white( 1 ) );
		//mesh1 = scene.make_mesh( scone::GetFolder( SCONE_GEOMETRY_FOLDER ) + "femur.vtp" );
		//mesh1.pos( vec3f( -0.5, 1, 0 ) );
		//mesh1.set_color( make_blue(), 1, 12, 0.5, 0 );
		//mesh2 = scene.make_sphere( 0.2f, vis::make_red(), 0.9f );
		//mesh2.pos( vec3f( 0.5f, 1, 0 ) );
	}

	StudioScene::~StudioScene() { }

	scone::StudioModel& StudioScene::CreateModel( const String& par_file )
	{
		model.reset();
		model = StudioModelUP( new StudioModel( scene, par_file ) );
		return *model;
	}

	void StudioScene::Update( TimeInSeconds v )
	{
		if ( model )
			model->UpdateVis( v );
	}

	scone::TimeInSeconds StudioScene::GetMaxTime()
	{
		return model->GetData().Back().GetTime();
	}
}

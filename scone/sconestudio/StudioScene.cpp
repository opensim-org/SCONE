#include "StudioScene.h"

using namespace vis;

namespace scone
{
	StudioScene::StudioScene()
	{
		// init scene
		scene.make_light( vec3f( -20, 80, 40 ), make_white( 0.8 ) );
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

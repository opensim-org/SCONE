#include "StudioManager.h"

using namespace vis;

namespace scone
{
	StudioManager::StudioManager()
	{
		// init scene
		scene.make_light( vec3f( -20, 80, 40 ), make_white( 0.8 ) );
	}

	StudioManager::~StudioManager() { }

	scone::StudioModel& StudioManager::CreateModel( const String& par_file )
	{
		model.reset();
		model = StudioModelUP( new StudioModel( scene, par_file ) );
		return *model;
	}

	void StudioManager::Update( TimeInSeconds v )
	{
		if ( model )
			model->UpdateVis( v );
	}

	scone::TimeInSeconds StudioManager::GetMaxTime()
	{
		return model->GetData().Back().GetTime();
	}
}

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
	}

	bool StudioScene::IsEvaluating()
	{
		return ( model && model->IsEvaluating() );
	}

	scone::TimeInSeconds StudioScene::GetMaxTime()
	{
		if ( model )
		{
			if ( model->IsEvaluating() )
				return model->GetSimModel().GetTime(); // TODO: this should be thread safe
			else
			{
				std::unique_lock< std::mutex > lock( model->GetDataMutex() );
				return model->GetData().IsEmpty() ? 0.0 : model->GetData().Back().GetTime();
			}
		}
		else return 0.0;
	}

	bool StudioScene::HasModel()
	{
		return static_cast< bool >( model );
	}
}

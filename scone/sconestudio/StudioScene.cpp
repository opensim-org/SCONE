#include "StudioScene.h"
#include "scone/core/system_tools.h"
#include "flut/math/math.hpp"
#include "scone/core/Profiler.h"

using namespace vis;

namespace scone
{
	StudioScene::StudioScene() :
	scene( true )
	{
		// init scene
		scene.add_light( vec3f( -20, 80, 40 ), make_white( 1 ) );
		scene.create_tile_floor( 64, 64, 1 );
	}

	StudioScene::~StudioScene() { }

	scone::StudioModel& StudioScene::CreateModel( const String& par_file, bool force_evaluation )
	{
		model.reset();
		model = StudioModelUP( new StudioModel( scene, par_file, force_evaluation ) );
		return *model;
	}

	void StudioScene::Update( TimeInSeconds t, bool update_vis )
	{
		SCONE_PROFILE_FUNCTION;
		if ( model )
		{
			if ( model->IsEvaluating() )
				model->EvaluateTo( t );

			if ( update_vis )
				model->UpdateVis( t );
		}
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
				return model->GetObjective().max_duration;
			else return model->GetData().IsEmpty() ? 0.0 : model->GetData().Back().GetTime();
		}
		else return 0.0;
	}

	bool StudioScene::HasModel()
	{
		return static_cast< bool >( model );
	}
}

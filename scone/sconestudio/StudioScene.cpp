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
	}

	scone::TimeInSeconds StudioScene::GetMaxTime()
	{
		if ( model )
			return model->GetData().IsEmpty() ? 0.0 : model->GetData().Back().GetTime();
		else return 0.0;
	}

	bool StudioScene::HasModel()
	{
		return static_cast< bool >( model );
	}
}

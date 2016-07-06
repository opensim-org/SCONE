#include "StudioManager.h"

using namespace vis;

namespace scone
{
	StudioManager::StudioManager()
	{
		// init scene
		auto b = scene.make_cube( vec3f( 0.5f, 0.7f, 1.5f ), make_red() );
		b.pos_ori( vec3f( 0, 3, 0 ), flut::math::make_quat_from_euler<float>( flut::math::degree( 45 ), flut::math::degree( 0 ), flut::math::degree( 0 ) ) );

		scene.make_light( vec3f( -20, 80, 40 ), make_white( 0.8 ) );

		int n = 10;
		for ( int i = 0; i < n; ++i )
		{
			auto s = scene.make_sphere( i * 0.02f + 0.2f, color( float( i ) / n, 0, 1 - float( i ) / n ), 0.9f );
			s.pos( vec3f( i * sin( i ) * 0.2, i, i * cos( i ) * 0.2 ) );
			spheres.push_back( s );
		}
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
		for ( size_t i = 0; i < spheres.size(); ++i )
		{
			spheres[ i ].pos( vec3f( i * sin( i ) * 0.2, v * i, i * cos( i ) * 0.2 ) );
		}

		if ( model )
			model->UpdateVis( v );
	}

	scone::TimeInSeconds StudioManager::GetMaxTime()
	{
		return model->GetData().Back().GetTime();
	}
}

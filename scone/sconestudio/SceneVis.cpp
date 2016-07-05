#include "SceneVis.h"

#include "OsgTools.h"

#include <osg/ShapeDrawable>
#include <osg/LightSource>
#include <osg/PositionAttitudeTransform>
#include <osgShadow/ShadowMap>
#include <osgShadow/ShadowVolume>
#include <osgShadow/SoftShadowMap>
#include "flut/math/quat.hpp"
#include "flut/math/angle.hpp"

using namespace osg;
using namespace simvis;

namespace scone
{
	SceneVis::SceneVis()
	{
		auto b = scene.make_cube( vec3f( 1, 2, 3 ), make_red() );
		b.pos_ori( vec3f( 0, 3, 0 ), flut::math::make_quat_from_euler<float>( flut::math::degree( 45 ), flut::math::degree( 0 ), flut::math::degree( 0 ) ) );

		scene.make_light( vec3f( -2, 8, 4 ), make_white( 0.8 ) );

	}

	SceneVis::~SceneVis() { }

	scone::ModelVis& SceneVis::CreateModel( sim::Model& m )
	{
		models.push_back( ModelVisUP( new ModelVis( m ) ) );
		auto& model = *models.back();
		return model;
	}

	void SceneVis::UpdateModels()
	{
		for ( auto& m : models )
			m->Update();
	}
}

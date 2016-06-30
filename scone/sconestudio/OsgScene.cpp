#include "OsgScene.h"

#include "OsgTools.h"

#include <osg/ShapeDrawable>
#include <osg/LightSource>
#include <osg/PositionAttitudeTransform>
#include <osgShadow/ShadowMap>
#include <osgShadow/ShadowVolume>
#include <osgShadow/SoftShadowMap>

using namespace osg;

namespace scone
{
	OsgScene::OsgScene()
	{
		root = new osgShadow::ShadowedScene;
		root_state = root->getOrCreateStateSet();

		/// setup shadows
		auto sm = new osgShadow::SoftShadowMap;
		sm->setTextureSize( osg::Vec2s( 1024, 1024 ) );
        root->setShadowTechnique( sm );

		auto ss = new osgShadow::ShadowSettings;
		ss->setCastsShadowTraversalMask( OsgCastShadowMask );
		ss->setReceivesShadowTraversalMask( OsgReceiveShadowMask );
		root->setShadowSettings( ss );

		// create floor
		ground = CreateOsgFloor( 50, 50, 1 );
		SetOsgShadowMask( ground, true, false );
		root->addChild( ground );

		auto sphere = CreateOsgBox( 0.1, 0.3, 0.5, OsgColor( 0xff0000 ) );
		SetOsgShadowMask( sphere, false, true );
		auto sphere_trans = new osg::PositionAttitudeTransform;
		sphere_trans->addChild( sphere );
		sphere_trans->setPosition( osg::Vec3( 0, 2, 0 ) );
		root->addChild( sphere_trans );

		auto axes = osgDB::readNodeFile( "resources/osg/axes.osgt" );
		SetOsgShadowMask( axes, false, false );
		root->addChild( axes );

		sky_light = CreateOsgLightSource( 1, osg::Vec4( -2, 8, 4, 1 ), osg::Vec4( 0.8, 0.8, 0.8, 1 ), 0.5 );
		root->addChild( sky_light );

		root_state->setMode( GL_LIGHT1, osg::StateAttribute::ON );
	}

	OsgScene::~OsgScene() { }

	scone::OsgModel& OsgScene::CreateModel( sim::Model& m )
	{
		models.push_back( std::make_unique< OsgModel >( m ) );
		auto& model = *models.back();

		root->addChild( model.GetOsgRoot() );
		return model;
	}

	void OsgScene::UpdateModels()
	{
		for ( auto& m : models )
			m->Update();
	}
}

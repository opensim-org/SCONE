#include "OsgScene.h"

#include <osg/ShapeDrawable>

using namespace osg;

namespace scone
{
	OsgScene::OsgScene()
	{
		root = new Group();

		ground = new Geode();
		ground->addDrawable( new osg::ShapeDrawable( new osg::Sphere( osg::Vec3( 0.0f, 0.0f, 0.0f ), 0.5 ) ) );
		ground->addDrawable( new osg::ShapeDrawable( new osg::Box( osg::Vec3( 0, -0.1, 0 ), 100, 0.1, 100 ) ) );

		root->addChild( ground );
		root->addChild( osgDB::readNodeFile( "resources/osg/axes.osgt" ) );
	}

	OsgScene::~OsgScene()
	{
	}

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

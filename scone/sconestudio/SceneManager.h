#pragma once

#include <osgGA/MultiTouchTrackballManipulator>
#include <osgDB/ReadFile>
#include <osgShadow/ShadowedScene>

#include "ModelVis.h"
#include "simvis/scene.h"

namespace scone
{
	class SceneManager 
	{
	public:
		SceneManager();
		virtual ~SceneManager();

		ModelVis& CreateModel( sim::Model& m );
		void Update( double v );
		void UpdateModels();

		osg::Group* GetOsgRoot() { return &scene.osg_root(); }

	private:
		simvis::scene scene;
		std::vector< ModelVisUP > models;
		std::vector< simvis::mesh > spheres;
	};
}

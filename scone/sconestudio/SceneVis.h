#pragma once

#include <osgGA/MultiTouchTrackballManipulator>
#include <osgDB/ReadFile>
#include <osgShadow/ShadowedScene>

#include "ModelVis.h"
#include "simvis/scene.h"

namespace scone
{
	class SceneVis 
	{
	public:
		SceneVis();
		virtual ~SceneVis();

		ModelVis& CreateModel( sim::Model& m );
		void UpdateModels();

		osg::Group* GetOsgRoot() { return &scene.osg_root(); }

	private:
		simvis::scene scene;
		std::vector< ModelVisUP > models;
	};
}

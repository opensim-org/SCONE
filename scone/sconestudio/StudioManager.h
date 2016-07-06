#pragma once

#include <osgGA/MultiTouchTrackballManipulator>
#include <osgDB/ReadFile>
#include <osgShadow/ShadowedScene>

#include "StudioModel.h"
#include "simvis/scene.h"

namespace scone
{
	class StudioManager 
	{
	public:
		StudioManager();
		virtual ~StudioManager();

		StudioModel& CreateModel( const String& par_file );
		StudioModel& GetModel() { return *model; }

		void Update( TimeInSeconds t );

		osg::Group* GetOsgRoot() { return &scene.osg_root(); }

	private:
		vis::scene scene;
		std::unique_ptr< StudioModel > model;
		std::vector< vis::mesh > spheres;
	};
}

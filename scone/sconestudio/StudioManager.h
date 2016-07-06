#pragma once

#include "StudioModel.h"
#include "simvis/scene.h"
#include <osg/Group>

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
	public:
		TimeInSeconds GetMaxTime();
	};
}

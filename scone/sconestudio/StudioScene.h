#pragma once

#include "StudioModel.h"
#include "simvis/scene.h"
#include <osg/Group>

namespace scone
{
	class StudioScene 
	{
	public:
		StudioScene();
		virtual ~StudioScene();

		StudioModel& CreateModel( const String& par_file );
		StudioModel& GetModel() { return *model; }

		void Update( TimeInSeconds t );

		osg::Group* GetOsgRoot() { return &scene.osg_group(); }

		bool IsEvaluating();
	private:
		vis::scene scene;
		std::unique_ptr< StudioModel > model;
		vis::mesh mesh1, mesh2;
		vis::path circle;
	public:
		TimeInSeconds GetMaxTime();
		bool HasModel();
	};
}

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

		StudioModel& CreateModel( const String& par_file, bool force_evaluation = false );
		StudioModel& GetModel() { return *model; }

		void Update( TimeInSeconds t, bool update_vis );

		osg::Group* GetOsgRoot() { return &scene.osg_group(); }

		bool IsEvaluating();
	private:
		vis::scene scene;
		std::unique_ptr< StudioModel > model;
		vis::mesh mesh1, mesh2;
		vis::trail circle;
	public:
		TimeInSeconds GetMaxTime();
		bool HasModel();
	};
}

#pragma once

#include <osgGA/MultiTouchTrackballManipulator>
#include <osgDB/ReadFile>
#include "OsgModel.h"

namespace scone
{
	class OsgScene 
	{
	public:
		OsgScene();
		virtual ~OsgScene();

		OsgModel& CreateModel( sim::Model& m );
		void UpdateModels();

	private:
		osg::Node* scene;
		std::vector< OsgModelUP > models;

	};
}

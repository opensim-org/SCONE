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

		osg::ref_ptr< osg::Group > GetOsgRoot() { return root; }

	private:
		osg::ref_ptr< osg::Group > root;
		osg::ref_ptr< osg::Geode > ground;
		std::vector< OsgModelUP > models;
	};
}

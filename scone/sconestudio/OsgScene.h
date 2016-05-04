#pragma once

#include <osgGA/MultiTouchTrackballManipulator>
#include <osgDB/ReadFile>

namespace scone
{
	class OsgScene 
	{
	public:
		OsgScene();
		virtual ~OsgScene();
		
	private:
		osg::Node* scene;
	};
}

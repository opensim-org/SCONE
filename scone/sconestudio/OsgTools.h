#pragma once

#include <osg/Geode>
#include <osg/ShapeDrawable>

namespace scone
{
	const int OsgReceiveShadowMask = 0x1;
	const int OsgCastShadowMask = 0x2;

	osg::Geode* CreateOsgFloor( int x_tiles, int z_tiles, float tile_width = 1.0f );
	osg::Geode* CreateOsgSphere( float radius, float detail );
	osg::Geode* CreateOsgBox( float x_len, float y_len, float z_len );
	osg::LightSource* CreateOsgLightSource( int num, const osg::Vec4& position, const osg::Vec4& color, float specular );
	void SetOsgShadowMask( osg::Node* n, bool receive, bool cast );
}

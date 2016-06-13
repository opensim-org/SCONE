#include "OsgCameraManipulator.h"

using namespace osg;

namespace scone
{
	
	OsgCameraManipulator::OsgCameraManipulator() : osgGA::OrbitManipulator()
	{
	}

	OsgCameraManipulator::~OsgCameraManipulator()
	{
	}

	void OsgCameraManipulator::rotateTrackball( const float px0, const float py0, const float px1, const float py1, const float scale )
	{
		// TODO: do something with this and perhaps other functions
		// TODO: get rid of 80s orbit animation

		osg::Vec3d axis;
		float angle;

		trackball( axis, angle, px0 + (px1-px0)*scale, 0, px0, 0 );

		Quat new_rotate;
		new_rotate.makeRotate( angle, axis );

		_rotation = _rotation * new_rotate;
	}

}

#include "OsgCameraManipulator.h"
#include "scone/core/math.h"

using namespace osg;

namespace scone
{
	
	OsgCameraManipulator::OsgCameraManipulator() : osgGA::OrbitManipulator(),
		orbit_yaw( 0 ),
		orbit_pitch( -10 )
	{
		setAllowThrow( false );
		_distance = 8;
		_center = Vec3d( 0, 1, 0 );

		updateRotation();

		osg::Vec3d eye, center, up;
		getTransformation( eye, center, up );
		setHomePosition( eye, center, up );
	}

	OsgCameraManipulator::~OsgCameraManipulator() {}

	bool OsgCameraManipulator::performMovementLeftMouseButton( const double eventTimeDelta, const double dx, const double dy )
	{
		orbit_pitch += Degree( pitch_scale * dy );
		orbit_yaw -= Degree( yaw_scale * dx );

		updateRotation();

		return true;
	}

	void OsgCameraManipulator::updateRotation()
	{
		auto yaw = osg::Quat( orbit_yaw.rad_value(), osg::Vec3d( 0, 1, 0 ) );
		auto pitch = osg::Quat( orbit_pitch.rad_value(), osg::Vec3d( 1, 0, 0 ) );
		_rotation = pitch * yaw;
	}

	bool OsgCameraManipulator::performMovementMiddleMouseButton( const double eventTimeDelta, const double dx, const double dy )
	{
	    zoomModel( dy * zoom_scale, true );
		return true;
	}

	bool OsgCameraManipulator::performMovementRightMouseButton( const double eventTimeDelta, const double dx, const double dy )
	{
		// pan model
		float scale = -pan_scale * _distance;
		panModel( dx * scale, dy * scale );
		return true;
	}
}

#include "OsgCameraManipulator.h"
#include "scone/core/math.h"
#include "QOsgViewer.h"

using namespace osg;

OsgCameraManipulator::OsgCameraManipulator( QOsgViewer* v ) :
	osgGA::OrbitManipulator(),
	orbit_yaw( 0 ),
	orbit_pitch( -5 ),
	viewer( v )
{
	setAllowThrow( false );
	_distance = 5;
	_center = Vec3d( 0, 1, 0 );

	updateRotation();

	osg::Vec3d eye, center, up;
	getTransformation( eye, center, up );
	setHomePosition( eye, center, up );
}

OsgCameraManipulator::~OsgCameraManipulator() {}

bool OsgCameraManipulator::performMovementLeftMouseButton( const double eventTimeDelta, const double dx, const double dy )
{
	orbit_pitch += degree( pitch_scale * dy );
	orbit_yaw -= degree( yaw_scale * dx );

	updateRotation();
	viewer->update(); // see if this can be done automatically through a callback

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
	zoomModel( dy * zoom_scale, false );
	viewer->update(); // see if this can be done automatically through a callback

	return true;
}

bool OsgCameraManipulator::performMovementRightMouseButton( const double eventTimeDelta, const double dx, const double dy )
{
	// pan model
	float scale = -pan_scale * _distance;
	panModel( dx * scale, dy * scale );
	viewer->update(); // see if this can be done automatically through a callback

	return true;
}

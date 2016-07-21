#pragma once

#include <osgGA/OrbitManipulator>
#include "flut/math/angle.hpp"

using flut::math::degree;

class QOsgViewer;

class osg_cam_man : public osgGA::OrbitManipulator
{
public:
	osg_cam_man( QOsgViewer* v );
	virtual ~osg_cam_man();
		
protected:
    virtual bool performMovementLeftMouseButton( const double eventTimeDelta, const double dx, const double dy );
    virtual bool performMovementMiddleMouseButton( const double eventTimeDelta, const double dx, const double dy );
    virtual bool performMovementRightMouseButton( const double eventTimeDelta, const double dx, const double dy );
	void updateRotation();

private:
	QOsgViewer* viewer;
	degree orbit_yaw;
	degree orbit_pitch;

	double pitch_scale = 100;
	double yaw_scale = 100;
	double pan_scale = 0.3;
	double zoom_scale = 1.0;
};

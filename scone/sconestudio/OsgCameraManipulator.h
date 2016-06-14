#pragma once

#include <osgGA/OrbitManipulator>
#include "scone/core/Angle.h"

namespace scone
{
	class OsgCameraManipulator : public osgGA::OrbitManipulator
	{
	public:
		OsgCameraManipulator();
		virtual ~OsgCameraManipulator();
		
	protected:
        virtual bool performMovementLeftMouseButton( const double eventTimeDelta, const double dx, const double dy );
        virtual bool performMovementMiddleMouseButton( const double eventTimeDelta, const double dx, const double dy );
        virtual bool performMovementRightMouseButton( const double eventTimeDelta, const double dx, const double dy );
		void updateRotation();

	private:
		Degree orbit_yaw;
		Degree orbit_pitch;

		double pitch_scale = 100;
		double yaw_scale = 100;
		double pan_scale = 0.3;
		double zoom_scale = 1.0;
	};
}

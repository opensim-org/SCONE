#pragma once

#include <osgGA/OrbitManipulator>

namespace scone
{
	class OsgCameraManipulator : public osgGA::OrbitManipulator
	{
	public:
		OsgCameraManipulator();
		virtual ~OsgCameraManipulator();
		
	protected:
		virtual void rotateTrackball( const float px0, const float py0, const float px1, const float py1, const float scale ) override;

	};
}

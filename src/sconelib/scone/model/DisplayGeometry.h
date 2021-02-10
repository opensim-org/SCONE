#pragma once

#include "scone/core/Vec3.h"
#include "scone/core/Quat.h"
#include "scone/core/types.h"
#include "xo/filesystem/path.h"

namespace scone
{
	struct DisplayGeometry
	{
		DisplayGeometry() = default;
		DisplayGeometry( const path& file, const Vec3& p, const Quat& q = Quat(), const Vec3& s = Vec3::diagonal( 1 ) ) :
			filename( file ),
			pos( p ),
			ori( q ),
			scale( s )
		{}

		xo::path filename;
		Vec3 pos;
		Quat ori;
		Vec3 scale;
	};
}

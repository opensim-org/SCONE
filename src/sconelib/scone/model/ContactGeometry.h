/*
** ContactGeometry.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see https://scone.software.
*/

#pragma once

#include "Body.h"
#include "scone/core/Vec3.h"
#include "scone/core/Quat.h"
#include "xo/shape/shape.h"

namespace scone
{
	struct ContactGeometry
	{
		ContactGeometry( const String& name, const Body& b, const xo::shape& s, const Vec3& p, const Quat& q ) :
			m_Name( name ),
			m_Body( b ),
			m_Shape( s ),
			m_Pos( p ),
			m_Ori( q )
		{}

		const String& m_Name;
		const Body& m_Body;
		xo::shape m_Shape;
		Vec3 m_Pos;
		Quat m_Ori;
	};
}

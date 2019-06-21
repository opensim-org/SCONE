#pragma once

#include "Body.h"
#include "scone/core/Vec3.h"
#include "scone/core/Quat.h"

namespace scone
{
	struct ContactGeometry
	{
		enum Shape { Unknown, Sphere, Box, Plane };
		ContactGeometry( const Body& b, const Vec3& p, double radius ) : m_Body( b ), m_Shape( Sphere ), m_Pos( p ), m_Scale( Vec3( radius, radius, radius ) ) {}
		ContactGeometry( const Body& b, Shape s, const Vec3& p, const Quat& q, const Vec3& scale ) : m_Body( b ), m_Shape( s ), m_Pos( p ), m_Ori( q ), m_Scale( scale ) {}
		const Body& m_Body;
		Shape m_Shape;
		Vec3 m_Pos;
		Quat m_Ori;
		Vec3 m_Scale;
	};
}

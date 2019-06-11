#include "stdafx.h"

#if 0
#include "Quat.h"
#include "Exception.h"

#include <cmath>

namespace scone
{
	const Quat Quat::ZERO( Real( 1 ), Real( 0 ), Real( 0 ), Real( 0 ) );

	Quat::Quat()
	{
		*this = ZERO;
	}

	Quat::Quat( Real w, Real x, Real y, Real z )
	{
		Set( w, x, y, z );
	}

	Quat::~Quat()
	{

	}

	Vec3 Quat::operator*( const Vec3& v ) const
	{
		// nVidia SDK implementation
		Vec3 uv, uuv;
		Vec3 qvec( X(), Y(), Z() );

		uv = qvec.GetCrossProduct( v );
		uuv = qvec.GetCrossProduct( uv );
		uv *= ( 2.0f * W() );
		uuv *= 2.0f;

		return v + uv + uuv;
	}

	Quat Quat::operator-() const
	{
		return Quat( W(), -X(), -Y(), -Z() );
	}

	Quat& Quat::Normalize()
	{
		Real len = sqrt( X() * X() + Y() * Y() + Z() * Z() + W() * W() );
		if ( len > std::numeric_limits<Real>::epsilon() )
		{
			Real invlen = 1.0 / len;
			X() *= invlen;
			Y() *= invlen;
			Z() *= invlen;
			W() *= invlen;
		}

		return (*this);
	}

	Quat Quat::GetNormalized() const
	{
		Quat q(*this);
		return q.Normalize();
	}

	Quat Quat::operator*( const Quat& other ) const
	{
		return Quat(
			W() * other.W() - X() * other.X() - Y() * other.Y() - Z() * other.Z(),
			W() * other.X() + X() * other.W() + Y() * other.Z() - Z() * other.Y(),
			W() * other.Y() + Y() * other.W() + Z() * other.X() - X() * other.Z(),
			W() * other.Z() + Z() * other.W() + X() * other.Y() - Y() * other.X()
			);
	}

	Real Quat::GetRoll() const
	{
		return atan2( 2 * ( X() * Y() + W() * Z() ), W() * W() + X() * X() - Y() * Y() - Z() * Z() );
	}

	Real Quat::GetPitch() const
	{
		return atan2( 2 * ( Y() * Z() + W() * X() ), W() * W() - X() * X() - Y() * Y() + Z() * Z() );
	}

	Real Quat::GetYaw() const
	{
		Real v = -2 * ( X() * Z() - W() * Y() );

		// make sure v is in range
		Restrain( v, Real( -1 ), Real( 1 ) );

		return asin( v );
	}

	Quat QuatFromSourceToTarget( const Vec3& source, const Vec3& target )
	{
		Vec3 s = source.GetNormalized();
		Vec3 t = target.GetNormalized();

		Vec3 c = s.GetCrossProduct( t );
		Real d = s.GetDotProduct( t );

		// check if vectors are the same
		if ( Equals( d, Real( 1 ), REAL_EPSILON * 3 ) )
		{
			// set to no rotation and return
			return Quat::ZERO;
		}

		// check for 180 degree rotation
		Real clen = c.GetLength();
		if ( clen <= REAL_EPSILON )
		{
			// pick an axis to rotate around
			Vec3 r = Vec3::UNIT_Y.GetCrossProduct( source );
			Real rlen = r.GetLength();

			if ( rlen <= REAL_EPSILON )
			{
				// bad luck, pick another axis
				r = Vec3::UNIT_X.GetCrossProduct( source );
				rlen = r.GetLength();
			}

			// normalize, set rotation and return
			r /= rlen;
			return QuatFromAxisAngle( r, Radian( REAL_PI ) );
		}

		// normalize c
		c /= clen;

		// get angle and set quaternion
		Real a = acos( d ) * Real( 0.5 );
		Real sa = sin( a );

		return Quat( cos( a ), c.X() * sa, c.Y() * sa, c.Z() * sa );
	}

	Quat QuatFromSourceToTarget( const Quat& source, const Quat& target )
	{
		return -source * target;
	}

	void Quat::ToAxisAngle( Vec3& axis, Real& angle ) const
	{
		// check if angle is non-zero
		Real len = sqrt( X() * X() + Y() * Y() + Z() * Z() );
		if ( len > REAL_EPSILON )
		{
			Real lim_w = W();
			Restrain( lim_w, -1.0, 1.0 );
			angle = Real( 2.0 * acos( lim_w ) );
			Real invlen = Real( 1.0 / len );

			axis.Set( X() * invlen, Y() * invlen, Z() * invlen );
		}
		else
		{
			// no rotation
			axis.Set( 1.0, 0, 0 );
			angle = 0;
		}
	}

	void Quat::ToAxisAngle2( Vec3& axis, Real& angle ) const
	{
		Real len = sqrt( X() * X() + Y() * Y() + Z() * Z() );
		angle = 2 * atan2( len, W() );
		if ( angle > REAL_EPSILON )
		{
			Real factor = 1.0 / sin( angle / 2 );
			axis.Set( factor * X(), factor * Y(), factor * Z() );
		}
		else axis.Set( 0, 0, 0 );
	}

	Vec3 Quat::ToExponentialMap2() const
	{
		Real len = sqrt( X() * X() + Y() * Y() + Z() * Z() );
		Real angle = 2 * atan2( len, W() );
		if ( angle > REAL_EPSILON )
		{
			Real factor = angle / sin( angle / 2 );
			return Vec3( factor * X(), factor * Y(), factor * Z() );
		}
		return Vec3::ZERO;
	}

	void Quat::ToAxes( Vec3& x, Vec3& y, Vec3& z ) const
	{
		x = (*this) * Vec3::UNIT_X;
		y = (*this) * Vec3::UNIT_Y;
		z = (*this) * Vec3::UNIT_Z;
	}

	Vec3 Quat::ToExponentialMap() const
	{
		Vec3 v;
		Real a;
		ToAxisAngle( v, a );

		return a * v;
	}

	void Quat::ToMatrix( Real m[3][3] ) const
	{
		Real fTx = 2.0f * X();
		Real fTy = 2.0f * Y();
		Real fTz = 2.0f * Z();
		Real fTwx = fTx * W();
		Real fTwy = fTy * W();
		Real fTwz = fTz * W();
		Real fTxx = fTx * X();
		Real fTxy = fTy * X();
		Real fTxz = fTz * X();
		Real fTyy = fTy * Y();
		Real fTyz = fTz * Y();
		Real fTzz = fTz * Z();

		m[0][0] = 1.0f - ( fTyy + fTzz );
		m[0][1] = fTxy - fTwz;
		m[0][2] = fTxz + fTwy;
		m[1][0] = fTxy + fTwz;
		m[1][1] = 1.0f - ( fTxx + fTzz );
		m[1][2] = fTyz - fTwx;
		m[2][0] = fTxz - fTwy;
		m[2][1] = fTyz + fTwx;
		m[2][2] = 1.0f - ( fTxx + fTyy );
	}

	void Quat::ToEuler( Real& x, Real& y, Real& z, EulerOrder eulerOrder ) const
	{
		// use matrix based method
		Real m[3][3];

		// convert to matrix
		ToMatrix( m );

		switch( eulerOrder )
		{
		case EULER_ORDER_XYZ:
			Restrain( m[0][2], Real( -1 ), Real( 1 ) );
			y = asin( m[0][2] );
			x = atan2( -m[1][2], m[2][2] );
			z = atan2( -m[0][1], m[0][0] );
			break;

		case EULER_ORDER_ZYX:
			Restrain( m[2][0], Real( -1 ), Real( 1 ) );
			y = asin( -m[2][0] );
			z = atan2( m[1][0], m[0][0] );
			x = atan2( m[2][1], m[2][2] );
			break;

		case EULER_ORDER_YXZ:
			Restrain( m[1][2], Real( -1 ), Real( 1 ) );
			x = asin( -m[1][2] );
			y = atan2( m[0][2], m[2][2] );
			z = atan2( m[1][0], m[1][1] );
			break;

		default:
			throw LogicException( "Euler order not supported" );
			break;
		}
	}

	Vec3 Quat::ToEuler( EulerOrder eulerOrder ) const
	{
		Vec3 v;
		ToEuler( v.X(), v.Y(), v.Z(), eulerOrder );
		return v;
	}

	Quat QuatFromExponentialMap( const Vec3& v )
	{
		Radian angle( v.GetLength() );

		if ( angle > std::numeric_limits<Real>::epsilon() )
		{
			return QuatFromAxisAngle( v / angle, angle );
		}
		else
		{
			return Quat::ZERO;
		}
	}

	Quat QuatFromAxisAngle( const Vec3& axis, const Radian& angle )
	{
		// axis must be of unit length! (this is not enforced)
		Real half_angle = Real( 0.5 ) * angle;
		Real half_sin = sin( half_angle );

		return Quat( cos( half_angle ), half_sin * axis.X(), half_sin * axis.Y(), half_sin * axis.Z() );
	}

	Quat QuatFromEuler( const Radian& xAngle, const Radian& yAngle, const Radian& zAngle, EulerOrder eulerOrder )
	{
		switch( eulerOrder )
		{
		case EULER_ORDER_XYZ:
			return QuatFromAxisAngle( Vec3::UNIT_X, xAngle ) * QuatFromAxisAngle( Vec3::UNIT_Y, yAngle ) * QuatFromAxisAngle( Vec3::UNIT_Z, zAngle );
			break;
		case EULER_ORDER_XZY:
			return QuatFromAxisAngle( Vec3::UNIT_X, xAngle ) * QuatFromAxisAngle( Vec3::UNIT_Z, zAngle ) * QuatFromAxisAngle( Vec3::UNIT_Y, yAngle );
			break;
		case EULER_ORDER_YXZ:
			return QuatFromAxisAngle( Vec3::UNIT_Y, yAngle ) * QuatFromAxisAngle( Vec3::UNIT_X, xAngle ) * QuatFromAxisAngle( Vec3::UNIT_Z, zAngle );
			break;
		case EULER_ORDER_YZX:
			return QuatFromAxisAngle( Vec3::UNIT_Y, yAngle ) * QuatFromAxisAngle( Vec3::UNIT_Z, zAngle ) * QuatFromAxisAngle( Vec3::UNIT_X, xAngle );
			break;
		case EULER_ORDER_ZXY:
			return QuatFromAxisAngle( Vec3::UNIT_Z, zAngle ) * QuatFromAxisAngle( Vec3::UNIT_X, xAngle ) * QuatFromAxisAngle( Vec3::UNIT_Y, yAngle );
			break;
		case EULER_ORDER_ZYX:
			return QuatFromAxisAngle( Vec3::UNIT_Z, zAngle ) * QuatFromAxisAngle( Vec3::UNIT_Y, yAngle ) * QuatFromAxisAngle( Vec3::UNIT_X, xAngle );
			break;
		default:
			throw LogicException( "Euler order not supported" );
			break;
		}
	}

	Quat QuatFromAxes( const Vec3& xvec, const Vec3& yvec, const Vec3& zvec )
	{
		// Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
		// article "Quaternion Calculus and Fast Animation".
		Vec3 xnorm = xvec.GetNormalized();
		Vec3 ynorm = yvec.GetNormalized();
		Vec3 znorm = zvec.GetNormalized();

		Quat q_ret;
		Real rot[3][3];

		rot[0][0] = xnorm.X();
		rot[1][0] = xnorm.Y();
		rot[2][0] = xnorm.Z();

		rot[0][1] = ynorm.X();
		rot[1][1] = ynorm.Y();
		rot[2][1] = ynorm.Z();

		rot[0][2] = znorm.X();
		rot[1][2] = znorm.Y();
		rot[2][2] = znorm.Z();

		Real fTrace = rot[0][0] + rot[1][1] + rot[2][2];
		Real fRoot;

		if ( fTrace > 0.0 )
		{
			// |w| > 1/2, may as well choose w > 1/2
			fRoot = sqrt( fTrace + 1.0f );  // 2w
			q_ret.W() = 0.5f* fRoot;
			fRoot = 0.5f / fRoot;  // 1/( 4w )
			q_ret.X() = ( rot[2][1] - rot[1][2] ) * fRoot;
			q_ret.Y() = ( rot[0][2] - rot[2][0] ) * fRoot;
			q_ret.Z() = ( rot[1][0] - rot[0][1] ) * fRoot;
		}
		else
		{
			// |w| <= 1/2
			static size_t s_iNext[3] = { 1, 2, 0 };
			size_t i = 0;
			if ( rot[1][1] > rot[0][0] )
				i = 1;
			if ( rot[2][2] > rot[i][i] )
				i = 2;
			size_t j = s_iNext[i];
			size_t k = s_iNext[j];

			fRoot = sqrt( rot[i][i] - rot[j][j] - rot[k][k] + 1.0f );
			Real* apkQuat[3] = { &q_ret.X(), &q_ret.Y(), &q_ret.Z() };
			*apkQuat[i] = 0.5f * fRoot;
			fRoot = 0.5f / fRoot;
			q_ret.W() = ( rot[k][j]-rot[j][k] ) * fRoot;
			*apkQuat[j] = ( rot[j][i] + rot[i][j] ) * fRoot;
			*apkQuat[k] = ( rot[k][i] + rot[i][k] ) * fRoot;
		}

		return q_ret;
	}
}
#endif
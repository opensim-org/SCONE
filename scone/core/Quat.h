#pragma once

#include "core.h"
#include "math.h"
#include "Vec3.h"

namespace scone
{
	enum EulerOrder
	{ 
		EULER_ORDER_XYZ,
		EULER_ORDER_XZY,
		EULER_ORDER_YXZ,
		EULER_ORDER_YZX,
		EULER_ORDER_ZXY,
		EULER_ORDER_ZYX
	};

	/** Class representing a Quaternion. */
	class CORE_API Quat
	{
	public:

		/** Constructors. */
		Quat();
		Quat( Real w, Real x, Real y, Real z );
		Quat( const Quat& other ) { *this = other; }

		/** Destructor. */
		virtual ~Quat();

		/** Assignment operator. */
		Quat& operator=( const Quat& other ) { memcpy( ( void* )m_V, ( void* )other.m_V, sizeof( Real[4] ) ); return *this; }

		/** Set quaternion value. */
		void Set( Real w, Real x, Real y, Real z ) { m_V[0] = w; m_V[1] = x; m_V[2] = y; m_V[3] = z; }

		/** Returns true if this is a zero rotation. */
		bool IsNull() const {
			return ( X() == Real( 0 ) ) && ( Y() == Real( 0 ) ) && ( Z() == Real( 0 ) ) && ( W() == Real( 1 ) );
		}

		/** Quaternion multiplication. */
		Quat operator*( const Quat& other ) const;

		/** Quaternion multiplication. */
		Quat& operator*=( const Quat& other )
		{
			( *this ) = ( *this ) * other;
			return ( *this );
		}

		/** Vector multiplication. */
		Vec3 operator*( const Vec3& v ) const;

		/** Negation (conjugate). */
		Quat operator-() const;

		/** Normalize. */
		Quat& Normalize();

		/** Get Normalized. */
		Quat GetNormalized() const;

		/** Convert to axis angle. */
		void ToAxisAngle( Vec3& axis, Real& angle ) const;
		void ToAxisAngle2( Vec3& axis, Real& angle ) const;

		/** Convert to exponential map. */
		Vec3 ToExponentialMap() const;
		Vec3 ToExponentialMap2() const;

		/** Convert to matrix. */
		void ToMatrix( Real m[3][3] ) const;

		/** Convert to axes. */
		void ToAxes( Vec3& x, Vec3& y, Vec3& z ) const;
		Vec3 GetAxisX() const { return (*this) * Vec3::UNIT_X; }
		Vec3 GetAxisY() const { return (*this) * Vec3::UNIT_Y; }
		Vec3 GetAxisZ() const { return (*this) * Vec3::UNIT_Z; }

		/** Convert quaternion to Euler Angles. */
		void ToEuler( Real& x, Real& y, Real& z, EulerOrder eulerOrder ) const;

		/** Convert quaternion to Euler Angles. */
		Vec3 ToEuler( EulerOrder eulerOrder = EULER_ORDER_XYZ ) const;

		/** Get Roll. */
		Real GetRoll() const;

		/** Get Pitch. */
		Real GetPitch() const;

		/** Get Yaw. */
		Real GetYaw() const;

		/** Static constant representing zero rotation. */
		static const Quat ZERO;

		Real& W() { return m_V[0]; }
		Real& X() { return m_V[1]; }
		Real& Y() { return m_V[2]; }
		Real& Z() { return m_V[3]; }

		const Real& W() const { return m_V[0]; }
		const Real& X() const { return m_V[1]; }
		const Real& Y() const { return m_V[2]; }
		const Real& Z() const { return m_V[3]; }

		const Real* Ptr() const { return m_V; }
		Real* Ptr() { return m_V; }
	private:

		Real m_V[4];
	};

	/** Get quaternion using an axis and an angle. */
	Quat CORE_API QuatFromAxisAngle( const Vec3& axis, const Radian& angle );

	/** Get quaternion using three axis vectors. */
	Quat CORE_API QuatFromAxes( const Vec3& x, const Vec3& y, const Vec3& z );

	/** Get quaternion using Euler angles. */
	Quat CORE_API QuatFromEuler( const Radian& xAngle, const Radian& yAngle, const Radian& zAngle, EulerOrder eulerOrder = EULER_ORDER_XYZ );

	/** Get quaternion to represent the rotation from source to target vector. */
	Quat CORE_API QuatFromSourceToTarget( const Vec3& source, const Vec3& target );

	/** Get quaternion to represent the rotation from source to target quaternion. */
	Quat CORE_API QuatFromSourceToTarget( const Quat& source, const Quat& target );

	/** Get quaternion from exponential map. */
	Quat CORE_API QuatFromExponentialMap(const Vec3& v);
};

#pragma once

#include "math.h"

#if 0
#include "platform.h"
#include <iostream>

namespace scone
{
	class CORE_API Vec3
	{
	public:
		Vec3() : x( Real( 0 ) ), y( Real( 0 ) ), z( Real( 0 ) ) { };
		Vec3( Real new_x, Real new_y, Real new_z ) : x( Real( new_x ) ), y( Real( new_y ) ), z( Real( new_z ) ) { };
		explicit Vec3( const float* v ) : x( Real( v[0] ) ), y( Real( v[1] ) ), z( Real( v[2] ) ) { };
		explicit Vec3( const double* v ) : x( Real( v[0] ) ), y( Real( v[1] ) ), z( Real( v[2] ) ) { };
		~Vec3() { };

		Vec3( const Vec3& other ) { *this = other; }
		Vec3& operator=( const Vec3& other ) { x = other.x; y = other.y; z = other.z; return *this; }

		// conversion members
		const Real* Ptr() const { return &x; }
		Real* Ptr() { return &x; }

		// dereferenced pointer cast
		operator Real*() { return Ptr(); }
		operator const Real*() const { return Ptr(); }

		// access members
		Real& X() { return x; }
		Real& Y() { return y; }
		Real& Z() { return z; }

		const Real& X() const { return x; }
		const Real& Y() const { return y; }
		const Real& Z() const { return z; }

		void Set( Real new_x, Real new_y, Real new_z ) { x = new_x; y = new_y; z = new_z; }
		Vec3& SetX( Real new_x ) { x = new_x; return *this; }
		Vec3& SetY( Real new_y ) { y = new_y; return *this; }
		Vec3& SetZ( Real new_z ) { z = new_z; return *this; }

		void ToXYZ( Real& new_x, Real& new_y, Real& new_z ) const { new_x = x; new_y = y; new_z = z; }

		/** Get the squared size of the vector. */
		Real GetSquaredLength() const;

		/** Get the size of the vector. */
		Real GetLength() const;

		/** Get a normalized copy of the vector. */
		Vec3 GetNormalized() const;

		/** Get a normalized copy of the vector. */
		bool Normalize();

		/** Check if the vector has zero length. */
		bool IsZeroLength();

		/** Comparison operators. */
		bool operator==( const Vec3& v ) const { return x == v.x && y == v.y && z == v.z; }
		bool operator!=( const Vec3& v ) const { return !(*this == v) ; }

		/** Multiply vector by scalar. */
		Vec3 operator*( Real s ) const;

		/** Multiply vector by scalar. */
		friend CORE_API Vec3 operator*( Real s, const Vec3& vec );

		/** Multiply (scale) vectors. */
		Vec3 operator*( const Vec3& vec ) const;

		/** Multiply vector by vector. */
		Vec3& operator*=( const Vec3& v );

		/** Multiply vector by scalar. */
		Vec3& operator*=( Real s );

		/** Divide vector by scalar. */
		Vec3& operator/=( Real s );

		/** Divide vector by scalar. */
		Vec3 operator/( Real s ) const;

		/** Add two vectors. */
		Vec3 operator+( const Vec3& v ) const;

		/** Add vector. */
		Vec3& operator+=( const Vec3& v );

		/** Subtract two vectors. */
		Vec3 operator-( const Vec3& v ) const;

		/** Subtract vector. */
		Vec3& operator-=( const Vec3& v );

		/** Negation. */
		Vec3 operator-() const;

		/** Compute cross product. */
		Vec3 operator^( const Vec3& v ) const { return GetCrossProduct(v); }
		Vec3 GetCrossProduct( const Vec3& v ) const;

		/** Compute dot product. */
		Real GetDotProduct( const Vec3& v ) const;

		/** Compute dot product. */
		Vec3 GetScaled( Real sx, Real sy, Real sz ) const;

		/** Get sum of elements. */
		double GetSum() const;

		/** Get vector 3D volume (X*Y*Z). */
		Real GetVolume() const;

		/** Get angle between this and other. */
		Real GetAngle( const Vec3& v ) const;

		/** Project onto plane. */
		Vec3& ProjectPlane( const Vec3& plane_normal );
		Vec3 GetPlaneProjection( const Vec3& plane_normal ) const;
		Vec3 GetVectorProjection( const Vec3& vec ) const;

		/** Get angle between v1 and v2 on plane defined by this normal. */
		Real GetAngleOnPlane( const Vec3& v1, const Vec3& v2 ) const;

		/** To string. */
		std::string ToString() const;
		Vec3& FromString( std::string& str );

		/** To stream. */
		friend std::ostream& operator<<( std::ostream& str, const Vec3& v ) {
			str << "[" << v.X() << " " << v.Y() << " " << v.Z() << "]";
			return str;
		}

		/** From stream. */
		friend std::istream& operator>>( std::istream& str, Vec3& v ) {
			char dummy;
			str >> dummy >> v.x >> v.y >> v.z >> dummy;
			return str;
		}

		/* Constants. */
		static const Vec3 ZERO;
		static const Vec3 UNIT_X;
		static const Vec3 UNIT_Y;
		static const Vec3 UNIT_Z;
		static const Vec3 NEG_UNIT_X;
		static const Vec3 NEG_UNIT_Y;
		static const Vec3 NEG_UNIT_Z;

		Real x;
		Real y;
		Real z;
	};

	CORE_API Real GetDotProduct( const Vec3& v1, const Vec3& v2 );
};
#endif
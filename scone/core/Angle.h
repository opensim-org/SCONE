#pragma once

#include "core.h"

namespace scone
{
	class Angle
	{
	public:
		static Angle Degree( Real deg ) { return Angle( DegToRad( deg ) ); }
		static Angle Radian( Real rad ) { return Angle( rad ); }

		Angle() {}
		Angle( const Angle& other ) : m_Value( other.m_Value ) {}
		Angle& operator=( const Angle& other ) { m_Value = other.m_Value; return *this; }

		// Angle is radian when cast as real
		operator const Real&() const { return GetRad(); }
		const Real& GetRad() const { return m_Value; }
		Real GetDeg() const { return RadToDeg( m_Value ); }

	private:
		// constructor is private so that users must use Deg or Rad to construct an Angle object
		Angle( Real value ) : m_Value( value ) { };
		Real m_Value;
	};

	//Angle Degree( Real deg ) { return Angle::Degree( deg ); }
	//Angle Radian( Real rad ) { return Angle::Radian( rad ); }

	// operators
	Real Sin( Angle& a ) { return sin( a.GetRad() ); }
	Real Cos( Angle& a ) { return cos( a.GetRad() ); }
	Real Tan( Angle& a ) { return tan( a.GetRad() ); }
	Angle ASin( Real a ) { return Angle::Radian( asin( a ) ); }
	Angle ACos( Real a ) { return Angle::Radian( acos( a ) ); }
	Angle ATan( Real a ) { return Angle::Radian( atan( a ) ); }
}

/*
** TimedValue.h
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

namespace scone
{
	template< typename T >
	class TimedValue
	{
	public:
		TimedValue( const T& value = T(0), TimeInSeconds time = 0.0 ) : m_Value( value ), m_Time( time ) { };
		operator const T&() const { return m_Value; }
		void Set( const T& value, TimeInSeconds time ) { if ( value != m_Value ) { m_Value = value; m_Time = time; } }
		TimeInSeconds GetTime() { return m_Time; }
			
	private:
		T m_Value;
		TimeInSeconds m_Time;
	};
}

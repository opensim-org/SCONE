#pragma once

namespace scone
{
	template< typename T >
	class TimedValue
	{
	public:
		TimedValue( const T& value = T(0), double time = 0.0 ) : m_Value( value ), m_Time( time ) { };

		operator const T&() const { return m_Value; }

		void Set( const T& value, double time ) {
			if ( value != m_Value )
			{
				m_Value = value;
				m_Time = time;
			}
		}

		double GetTime() { return m_Time; }
			
	private:
		T m_Value;
		double m_Time;
	};
}

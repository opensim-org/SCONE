#pragma once

#include <map>

namespace scone
{
	template< typename T >
	class EnumStringMap
	{
	public:
		typedef std::map< T, String > MapType;

		EnumStringMap( MapType& init_map ) : m_Map( init_map ) { };
		EnumStringMap( MapType::iterator i1, MapType::iterator i2 ) : m_Map( i1, i2 ) { };
		virtual ~EnumStringMap() { };

		const String& GetString( T e ) { return m_Map[ e ]; }
		T GetEnum( const String& s )
		{
			auto iter = std::find_if( m_Map.begin(), m_Map.end(), [&](const String& v) { return v == s; } );
			if ( iter != m_Map.end() )
				return iter->first;
			else return T(-1);
		}
			
	private:
		MapType m_Map;
	};
}

/*
** StringMap.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include <algorithm>
#include <map>
#include "platform.h"
#include "string_tools.h"


namespace scone
{
	template< typename T >
	class StringMap
	{
	public:
		// clumsy constructor because initializer lists aren't supported in VS2010
		StringMap(
			T e1 = T(0), const char* s1 = nullptr,
			T e2 = T(0), const char* s2 = nullptr,
			T e3 = T(0), const char* s3 = nullptr,
			T e4 = T(0), const char* s4 = nullptr,
			T e5 = T(0), const char* s5 = nullptr,
			T e6 = T(0), const char* s6 = nullptr,
			T e7 = T(0), const char* s7 = nullptr,
			T e8 = T(0), const char* s8 = nullptr,
			T e9 = T(0), const char* s9 = nullptr
			)
		{
			if ( s1 ) m_Map[ e1 ] = s1;
			if ( s2 ) m_Map[ e2 ] = s2;
			if ( s3 ) m_Map[ e3 ] = s3;
			if ( s4 ) m_Map[ e4 ] = s4;
			if ( s5 ) m_Map[ e5 ] = s5;
			if ( s6 ) m_Map[ e6 ] = s6;
			if ( s7 ) m_Map[ e7 ] = s7;
			if ( s8 ) m_Map[ e8 ] = s8;
			if ( s9 ) m_Map[ e9 ] = s9;
		};

		virtual ~StringMap() { };

		const String& GetString( T e ) const {
			auto iter = m_Map.find( e );
			SCONE_ERROR_IF( iter == m_Map.end(), "Invalid value: " + to_str( e ) );
			return iter->second;
		}

		T GetValue( const String& s ) const
		{
			auto iter = std::find_if( m_Map.begin(), m_Map.end(), [&]( const typename MapType::value_type& v) { return v.second == s; } );
			SCONE_ERROR_IF( iter == m_Map.end(), "Invalid string: " + s );
			return iter->first;
		}

		size_t GetSize() const { return m_Map.size(); }
			
	private:
		typedef std::map< T, String > MapType;
		MapType m_Map;
	};

	typedef StringMap< index_t > StringIndexMap;
}

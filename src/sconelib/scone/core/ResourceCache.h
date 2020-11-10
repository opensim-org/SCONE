/*
** ResourceCache.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include <map>
#include <string>

#include "Exception.h"
#include <mutex>

namespace scone
{
	template< typename T >
	class ResourceCache
	{
	public:
		ResourceCache() { };
		~ResourceCache() { };
		std::unique_ptr< T > CreateCopy( const String& name )
		{
			std::lock_guard< std::mutex > lock( m_Mutex );

			auto it = m_Resources.find( name );
			if ( it == m_Resources.end() )
			{
				T* r = CreateFirst( name );
				m_Resources[ name ] = std::unique_ptr< T >( r );
				it = m_Resources.find( name );
			}
			return std::make_unique<T>( *it->second );
		}
			
	private:
		T* CreateFirst( const String& name ) { SCONE_THROW_NOT_IMPLEMENTED; }
		std::mutex m_Mutex;
		std::map< std::string, std::unique_ptr< T > > m_Resources;
	};
}

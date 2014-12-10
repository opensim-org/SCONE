#pragma once

#include "core.h"
#include <map>
#include "PropNode.h"

namespace scone
{
	/// Get Single Factory instance
	CORE_API class Factory& GetFactory();

	/// Factory class
	class Factory
	{
	public:
		Factory() { };
		virtual ~Factory() { };

		template< typename T >
		void RegisterType( const String& type, T* (*func)(void) )
		{
			m_CreateFuncs[ GetFullTypeName< T >( type ) ] = (void*(*)(void))func;
		}

		template< typename T >
		T* Create( const String& type )
		{
			auto iter = m_CreateFuncs.find( GetFullTypeName< T >( type ) );
			if ( iter != m_CreateFuncs.end() )
			{
				// create the item
				return ((T*(*)(void))iter->second)();
			}
			else SCONE_THROW( "Could not find type " + type );
		}

	private:
		std::map< String, void*(*)(void) > m_CreateFuncs;
		template< typename T >
		String GetFullTypeName( const String& type ) { return String( typeid( T ).name() ) + "." + type; }
	};
}

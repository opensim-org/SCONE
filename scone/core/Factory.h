#pragma once

#include "core.h"
#include <map>
#include "PropNode.h"

namespace scone
{
	class Factory
	{
	public:
		Factory() { };
		virtual ~Factory() { };

		template< typename T >
		void RegisterType( const String& type, T* (*func)(void) )
		{
			m_CreateFuncs[ typeid(T).name() + type ] = (void*(*)(void))func;
		}

		template< typename T >
		T* Create( const PropNode& props )
		{
			return Create< T >( props.GetStr( "type" ) );
		}

		template< typename T >
		T* Create( const String& type )
		{
			String full_name = typeid(T).name() + type;
			auto iter = m_CreateFuncs.find( full_name );
			if ( iter != m_CreateFuncs.end() )
			{
				// create the item
				return ((T*(*)(void))iter->second)();
			}
			else SCONE_THROW( "Could not find type " + type );
		}

	private:
		std::map< String, void*(*)(void) > m_CreateFuncs;
	};

	CORE_API Factory& GetFactory();

	//template< typename T >
	//Factory< T >& GetFactory() { SCONE_THROW( "Factory is not available" ); }
}

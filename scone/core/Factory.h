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

		template< typename Base, typename Derived >
		void Register()
		{
			m_CreateFuncs[ GetFullTypeName< Base >( GetCleanClassName< Derived >() ) ] = (void*(*)(void))Derived::Create;
		}

		template< typename T >
		std::unique_ptr< T > Create( const String& type )
		{
			auto iter = m_CreateFuncs.find( GetFullTypeName< T >( type ) );
			if ( iter != m_CreateFuncs.end() )
			{
				// create the item
				return std::unique_ptr< T >( ( ( T*(*)(void) )iter->second )() );
			}
			else SCONE_THROW( "Unknown type " + type + ", make sure you call " + type + "::RegisterFactory()" );
		}

	private:
		std::map< String, void*(*)(void) > m_CreateFuncs;
		template< typename T >
		String GetFullTypeName( const String& type ) { return String( typeid( T ).name() ) + "-->" + type; }

		template< typename T >
		String GetCleanClassName()
		{
			String str = typeid( T ).name();
			size_t pos = str.find_last_of(": ");
			if (pos != std::string::npos) str = str.substr(pos + 1);
			return str;
		}
	};

	/// Factoryable class
	template< typename Base, typename Derived >
	class Factoryable
	{
	public:
		static Base* Create() { return new Derived; }
		static void RegisterFactory() { GetFactory().Register< Base, Derived >(); }
	};
}

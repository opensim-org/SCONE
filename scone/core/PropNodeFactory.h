#pragma once

#include "core.h"
#include <map>
#include "PropNode.h"

namespace scone
{
	/// Get Single Factory instance
	CORE_API class PropNodeFactory& GetFactory();

	/// Factory class
	class PropNodeFactory
	{
	public:
		PropNodeFactory() { };
		virtual ~PropNodeFactory() { };

		template< typename Base, typename Derived >
		void Register( const String& name = "" )
		{
			m_CreateFuncs[ GetFullTypeName< Base >( name.empty() ? GetCleanClassName< Derived >() : name ) ] = (void*(*)( const PropNode& ))Derived::Create;
		}

		template< typename T, typename P1 >
		std::unique_ptr< T > Create( const String& type, const P1& props )
		{
			auto iter = m_CreateFuncs.find( GetFullTypeName< T >( type ) );
			if ( iter != m_CreateFuncs.end() )
			{
				// create the item
				return std::unique_ptr< T >( ( ( T*(*)( const P1& ) )iter->second )( props ) );
			}
			else SCONE_THROW( "Unknown type " + type + ", make sure you call " + type + "::RegisterFactory()" );
		}

	private:
		std::map< String, void*(*)( const PropNode& ) > m_CreateFuncs;

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
		static Base* Create( const PropNode& props ) { return new Derived( props ); }
		static void RegisterFactory( const String& name = "" ) { GetFactory().Register< Base, Derived >( name ); }
	};
}

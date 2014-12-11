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
		String GetFullTypeName( const String& type ) { return String( typeid( T ).name() ) + "-->" + type; }

		template< typename T >
		String GetCleanClassName()
		{
			String str = typeid( T ).name();
			size_t pos1 = str.find_last_of("::");
			if (pos1 != std::string::npos) str = str.substr(pos1 + 1);
			size_t pos2 = str.find_last_of(" ");
			if (pos2 != std::string::npos) str = str.substr(pos2 + 1);
			return str;
		}
	};

	/// Factoryable registerer
	template< typename T >
	struct FactoryableRegisterer
	{
		FactoryableRegisterer() { T::RegisterFactory(); }
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

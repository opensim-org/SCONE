#pragma once

#include "core.h"
#include <map>
#include "PropNode.h"

namespace scone
{
	/// Get Single Factory instance
	CORE_API class Factory& GetFactory();

	// generate create and register functions in class
	#define SCONE_GENERATE_FACTORY_MEMBERS( _class_ ) \
	static _class_* FactoryCreate() { return new _class_; } \
	static void FactoryRegister() { GetFactory().RegisterType( #_class_, FactoryCreate ); }

	/// Factory class
	class Factory
	{
	public:
		Factory() { };
		virtual ~Factory() { };

		template< typename T >
		void RegisterType( const String& type, T* (*func)(void) )
		{
			m_CreateFuncs[ type ] = (void*(*)(void))func;
		}

		template< typename T >
		T* Create( const String& type )
		{
			auto iter = m_CreateFuncs.find( type );
			if ( iter != m_CreateFuncs.end() )
			{
				// create the item
				// TODO: validate type using RTTI?
				return ((T*(*)(void))iter->second)();
			}
			else SCONE_THROW( "Could not find type " + type );
		}

	private:
		std::map< String, void*(*)(void) > m_CreateFuncs;
	};
}

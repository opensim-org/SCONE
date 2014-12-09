#pragma once

#include "core.h"

namespace scone
{
	template< typename T >
	class Factory
	{
	public:
		typedef T* (*CreateFunc)(void);

		Factory() { };
		virtual ~Factory() { };

		void RegisterType( const String& type, CreateFunc func )
		{
			m_CreateFuncs[ type ] = func;
		}

		T* Create( const String& type )
		{
			auto iter = m_CreateFuncs.find( type );
			if ( iter != m_CreateFuncs.end() )
			{
				// create the item
				return iter->second();
			}
			else SCONE_THROW( "Could not find type " + type );
		}

	private:
		std::map< String, CreateFunc > m_CreateFuncs;
	};
}

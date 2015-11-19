#pragma once

#include <boost/function.hpp>
#include <boost/functional/factory.hpp>
#include <map>

#include "core.h"
#include "PropNode.h"
#include "tools.h"

#define DECLARE_FACTORY( _call_, _base_, _args_ ) \
	typedef Factory< boost::function< _base_*_args_ > > _base_##Factory; \
	_call_ _base_##Factory& Get##_base_##Factory(); \
	_call_ std::unique_ptr< _base_ > Create##_base_ _args_;

#define SCONE_DEFINE_FACTORY_HPP_2( _call_, _base_, _arg1_, _arg2_ ) \
	typedef Factory< boost::function< _base_*( _arg1_, _arg2_ ) > > _base_##Factory; \
	_call_ _base_##Factory& Get##_base_##Factory(); \
	_call_ std::unique_ptr< _base_ > _args_

#define SCONE_DEFINE_FACTORY_CPP_2( _call_, _base_, _arg1_, _arg2_ ) \
	static _base_##Factory g_##_base_##Factory; \
	_call_ _base_##Factory& Get##_base_##Factory() { return g_##_base_##Factory; } \
	_call_ std::unique_ptr< _base_ > Create##_base_( _arg1_ p1, _arg2_ p2 ) { return std::unique_ptr< _base_ >( Get##_base_##Factory().Create( p1 )( p1, p2 ) ); }

namespace scone
{
	template< typename F >
	class Factory
	{
	public:
		F& Create( const String& name )
		{
			auto it = m_CreateFuncs.find( name );
			if ( it != m_CreateFuncs.end() )
				return it->second;
			else SCONE_THROW( "Unknown type: " + name );
		}

		// TODO: use variadic functions for neater factories
		F& Create( const PropNode& props )
		{
			F& f = Create( props.GetStr( "type" ) );
			props.Touch();
			props.GetChild( "type" ).Touch();
			return f;
		}

		template< typename C >
		void Register( const String& name = GetCleanClassName< C >() )
		{
            std::cout << name << std::endl;
			m_CreateFuncs[ name ] = boost::factory< C* >();
		}

	private:
		std::map< String, F > m_CreateFuncs;
	};
}

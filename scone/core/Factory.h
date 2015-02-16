#pragma once

#include <boost/function.hpp>
#include <boost/functional/factory.hpp>
#include <map>

namespace scone
{
	#define DECLARE_FACTORY( _basetype_, _args_ ) \
		typedef Factory< boost::function< _basetype_*_args_ > > _basetype_##Factory;

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

		F& Create( const PropNode& props )
		{
			F& f = Create( props.GetStr( "type" ) );
			props.SetFlag();
			props.GetChild( "type" ).SetFlag();
			return f;
		}

		template< typename C >
		void Register( const String& name )
		{
			m_CreateFuncs[ name ] = boost::factory< C* >();
		}

	private:
		std::map< String, F > m_CreateFuncs;
	};
}

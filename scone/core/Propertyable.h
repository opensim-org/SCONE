#pragma once
#include "PropNode.h"
#include <string>
#include "Factory.h"

namespace scone
{
	namespace factory { }

	class CORE_API Propertyable
	{
	public:
		Propertyable();
		virtual ~Propertyable();

		virtual void ProcessProperties( const PropNode& props ) = 0;

	protected:
		// process Propertyable type
		void ProcessProperty( const PropNode& prop, Propertyable& var, const String& name )
		{
			if ( prop.HasKey( name ) )
				var.ProcessProperties( prop.GetChild( name ) );
		}

		// process shared_ptr type (requires factory definition)
		template< typename T >
		void ProcessProperty( const PropNode& prop, std::shared_ptr< T >& var, const String& name )
		{
			if ( prop.HasKey( name ) )
			{
				var = std::shared_ptr< T >( GetFactory().Create< T >( prop.GetChild( name ).GetStr( "type" ) ) );
				ProcessProperty( prop, *var, name );
			}
		}

		// process vector< shared_ptr > type (requires factory definition)
		template< typename T >
		void ProcessProperty( const PropNode& prop, std::vector< std::shared_ptr< T > >& var, const String& name )
		{ 
			if ( prop.HasKey( name ) )
			{
				//PropNode& node = prop.GetChild( name );
				//var.clear();
				//for ( auto iter = node.Begin(); iter != node.End(); ++iter )
				//	vec.push_back( std::shared_ptr< T >( factory::Create< T >( *iter->second ) ) );
			}
		}

		// process fundamental types and String
		template< typename T >
		void ProcessProperty( const PropNode& prop, T& var, const String& name, typename std::enable_if< std::is_fundamental< T >::value || std::is_same< T, String >::value >::type* = 0  )
		{
			if ( prop.HasKey( name ) )
				var = prop.Get< T >( name );
		}
	};

	// convenience macro that automatically derives name from variable name
	#define PROCESS_PROPERTY( _prop_, _var_ ) ProcessProperty( _prop_, _var_, GetCleanVarName( #_var_ ) )
}

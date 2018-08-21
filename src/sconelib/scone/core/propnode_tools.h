#pragma once

#include "PropNode.h"
#include "Log.h"

// convenience macro that automatically derives name from variable name
#define INIT_PROPERTY( _prop_, _var_, _default_ ) InitFromPropNodeChild<decltype( _var_ )>( _prop_, _var_, GetCleanVarName( #_var_ ), _default_ )
#define INIT_PROPERTY_REQUIRED( _prop_, _var_ ) InitFromPropNodeChild<decltype( _var_ )>( _prop_, _var_, GetCleanVarName( #_var_ ) )
#define INIT_PROPERTY_NAMED( _prop_, _var_, _name_, _default_ ) InitFromPropNodeChild<decltype( _var_ )>( _prop_, _var_, _name_, _default_ )

namespace scone
{
	SCONE_API String GetCleanVarName( const String& str );

	// process fundamental types and String
	template< typename T >
	void InitFromPropNode( const PropNode& prop, T& var )
	{
		var = prop.get< T >();
	}

	// process vector< > type (NOT TESTED)
	template< typename T >
	void InitFromPropNode( const PropNode& prop, std::vector< T >& vec )
	{ 
		vec.clear();
		for ( auto iter = prop.begin(); iter != prop.end(); ++iter )
		{
			vec.push_back( T() );
			InitFromPropNode( iter->second, vec.back() );
		}
	}

	// process named property type
	template< typename T >
	void InitFromPropNodeChild( const PropNode& prop, T& var, const String& name )
	{
		InitFromPropNode< T >( prop.get_child( name ), var );
	}

	// process named property type with default argument
	template< typename T >
	void InitFromPropNodeChild( const PropNode& prop, T& var, const String& name, const T& default_value )
	{
		if ( prop.has_key( name ) )
			InitFromPropNode( prop.get_child( name ), var );
		else var = T( default_value );
	}

	// log unused parameters
	SCONE_API void LogUntouched( const PropNode& p, log::Level level = log::WarningLevel, size_t depth = 0 );
}

#pragma once

#include "PropNode.h"
#include "string_tools.h"
#include "Log.h"

// convenience macro that automatically derives name from variable name
#define INIT_PROPERTY( _prop_, _var_, _default_ ) InitFromPropNodeChild( _prop_, _var_, GetCleanVarName( #_var_ ), _default_ )
#define INIT_PROPERTY_REQUIRED( _prop_, _var_ ) InitFromPropNodeChild( _prop_, _var_, GetCleanVarName( #_var_ ) )
#define INIT_PROPERTY_NAMED( _prop_, _var_, _name_, _default_ ) InitFromPropNodeChild( _prop_, _var_, _name_, _default_ )

// define a variable and init from props
#define DECLARE_AND_INIT( _prop_, _type_, _var_, _default_ ) _type_ _var_; InitFromPropNodeChild( _prop_, _var_, GetCleanVarName( #_var_ ), (_type_) _default_ )

namespace scone
{
	// process fundamental types and String
	template< typename T >
	void InitFromPropNode( const PropNode& prop, T& var )
	{
		var = prop.GetValue< T >();
	}

	// process vector< > type (NOT TESTED)
	template< typename T >
	void InitFromPropNode( const PropNode& prop, std::vector< T >& vec )
	{ 
		vec.clear();
		for ( auto iter = prop.Begin(); iter != prop.End(); ++iter )
		{
			vec.push_back( T() );
			InitFromPropNode( iter->second, vec.back() );
		}
	}

	// process named property type
	template< typename T >
	void InitFromPropNodeChild( const PropNode& prop, T& var, const String& name )
	{
		InitFromPropNode< T >( prop.GetChild( name ), var );
	}

	// process named property type with default argument
	template< typename T >
	void InitFromPropNodeChild( const PropNode& prop, T& var, const String& name, const T& default_value )
	{
		if ( prop.HasKey( name ) )
			InitFromPropNode( prop.GetChild( name ), var );
		else var = T( default_value );
	}

	// log unused parameters
	void SCONE_API LogUntouched( const PropNode& p, log::Level level = log::WarningLevel, size_t depth = 0 );
}

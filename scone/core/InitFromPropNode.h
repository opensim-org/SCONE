#pragma once

#include "PropNode.h"
#include <string>
#include "tools.h"

// convenience macro that automatically derives name from variable name
#define INIT_PROPERTY( _prop_, _var_, _default_ ) InitFromPropNodeChild( _prop_, _var_, GetCleanVarName( #_var_ ), _default_ )
#define INIT_PROPERTY_REQUIRED( _prop_, _var_ ) InitFromPropNodeChild( _prop_, _var_, GetCleanVarName( #_var_ ) )
#define INIT_PROPERTY_NAMED( _prop_, _var_, _name_, _default_ ) InitFromPropNodeChild( _prop_, _var_, _name_, _default_ )

//#define CONSTRUCT_FROM_PROP( _prop_, _var_ ) ConstructFromPropNode( _prop_.GetChild( GetCleanVarName( #_var_ ) ), _var_ );


// define a variable and init from props
#define DECLARE_AND_INIT( _prop_, _type_, _var_, _default_ ) _type_ _var_; InitFromPropNodeChild( _prop_, _var_, GetCleanVarName( #_var_ ), (_type_) _default_ )

namespace scone
{
	//class Propertyable
	//{
	//};

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

	// process fundamental types and String
	template< typename T >
	void InitFromPropNode( const PropNode& prop, T& var )
	{
		var = prop.GetValue< T >();
	}

	//// process propertyable types
	//template< typename T >
	//void InitFromPropNode( const PropNode& prop, T& var, typename std::enable_if< std::is_base_of< Propertyable, T >::value >::type* = 0 )
	//{
	//	var = T( prop );
	//}

	//// process propertyable types and String
	//template< typename T >
	//void ConstructFromPropNode( const PropNode& prop, T& var )
	//{
	//	var = T( prop );
	//}

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
}

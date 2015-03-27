#pragma once

#include "PropNode.h"
#include <string>

namespace scone
{
	// process named property type
	template< typename T >
	void InitFromPropNodeChild( const PropNode& prop, T& var, const String& name )
	{
		if ( prop.HasKey( name ) )
			InitFromPropNode< T >( prop.GetChild( name ), var );
		else SCONE_THROW( "Could not find key: " + name );
	}

	// process named property type with default argument
	template< typename T >
	void InitFromPropNodeChild( const PropNode& prop, T& var, const String& name, const T& default_value )
	{
		if ( prop.HasKey( name ) )
			InitFromPropNode( prop.GetChild( name ), var );
		else var = T( default_value );
	}

	// process String type
	//inline void InitFromPropNode( const PropNode& prop, String& var )
	//{
	//	var = prop.GetValue();
	//	prop.SetFlag();
	//}

	// process fundamental types and String
	template< typename T >
	void InitFromPropNode( const PropNode& prop, T& var )
	{
		var = prop.GetValue< T >();
		prop.SetFlag();
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
		prop.SetFlag();
	}

	//// process pointer type
	//template< typename T >
	//void InitFromPropNode( const PropNode& prop, std::unique_ptr< T >& var )
	//{
	//	var = GetFactory().Create< T >( prop.GetStr( "type" ), prop );
	//	prop.SetFlag();
	//	prop.GetChild( "type" ).SetFlag();
	//}

	//// create function (TODO: remove?)
	//template< typename T >
	//std::unique_ptr< T > CreateFromPropNode( const PropNode& prop )
	//{
	//	std::unique_ptr< T > var( GetFactory().Create< T >( prop.GetStr( "type" ), prop ) );
	//	prop.SetFlag();
	//	prop.GetChild( "type" ).SetFlag();

	//	return var;
	//}

	// convenience macro that automatically derives name from variable name
	#define INIT_FROM_PROP( _prop_, _var_, _default_ ) InitFromPropNodeChild( _prop_, _var_, GetCleanVarName( #_var_ ), _default_ )
	#define INIT_FROM_PROP_REQUIRED( _prop_, _var_ ) InitFromPropNodeChild( _prop_, _var_, GetCleanVarName( #_var_ ) )
	#define INIT_FROM_PROP_NAMED( _prop_, _var_, _name_, _default_ ) InitFromPropNodeChild( _prop_, _var_, _name_, _default_ )

	// define a variable and init from props
	#define DECLARE_AND_INIT( _prop_, _type_, _var_, _default_ ) _type_ _var_; InitFromPropNodeChild( _prop_, _var_, GetCleanVarName( #_var_ ), (_type_) _default_ )
}

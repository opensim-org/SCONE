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

	// process fundamental types and String
	template< typename T >
	void InitFromPropNode( const PropNode& prop, T& var, typename std::enable_if< std::is_fundamental< T >::value || std::is_same< T, String >::value >::type* = 0  )
	{
		var = prop.Get< T >();
		prop.SetFlag();
	}

	// process Propertyable type
	inline void InitFromPropNode( const PropNode& prop, String& var )
	{
		var = prop.GetValue();
		prop.SetFlag();
	}

	// process vector< unique_ptr > type (requires factory definition)
	//template< typename T >
	//void InitFromPropNode( const PropNode& prop, std::vector< std::unique_ptr< T > >& vec )
	//{ 
	//	vec.clear();
	//	for ( auto iter = prop.Begin(); iter != prop.End(); ++iter )
	//		vec.push_back( CreateFromPropNode< T >( *iter->second ) );
	//	prop.SetFlag();
	//}

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
	#define INIT_FROM_PROP_NAMED( _prop_, _var_, _name_, _default_ ) InitFromPropNodeChild( _prop_, _var_, _name_, _default_ )
}

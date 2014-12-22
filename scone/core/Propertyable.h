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
	};

	// process named property type
	template< typename T >
	void ProcessPropNode( const PropNode& prop, T& var, const String& name )
	{
		if ( prop.HasKey( name ) )
			ProcessPropNode( prop.GetChild( name ), var );
		else SCONE_THROW( "Could not find key: " + name );
	}

	// process named property type with default argument
	template< typename T >
	void ProcessPropNode( const PropNode& prop, T& var, const String& name, const T& default_value )
	{
		if ( prop.HasKey( name ) )
			ProcessPropNode( prop.GetChild( name ), var );
		else var = T( default_value );
	}

	// process Propertyable type
	inline void ProcessPropNode( const PropNode& prop, Propertyable& var )
	{
		var.ProcessProperties( prop );
	}

	// process unique_ptr type (requires factory definition)
	template< typename T >
	void ProcessPropNode( const PropNode& prop, std::unique_ptr< T >& var )
	{
		var = std::unique_ptr< T >( GetFactory().Create< T >( prop.GetStr( "type" ) ) );
		ProcessPropNode( prop, *var );
	}

	// process shared_ptr type (requires factory definition)
	template< typename T >
	void ProcessPropNode( const PropNode& prop, std::shared_ptr< T >& var )
	{
		var = std::shared_ptr< T >( GetFactory().Create< T >( prop.GetStr( "type" ) ) );
		ProcessPropNode( prop, *var );
	}

	// process vector< unique_ptr > type (requires factory definition)
	template< typename T >
	void ProcessPropNode( const PropNode& prop, std::vector< std::unique_ptr< T > >& var )
	{ 
		SCONE_THROW_NOT_IMPLEMENTED; // TODO: must be tested

		var.resize( prop.GetChildren().size(), nullptr );

		for ( auto iter = node.Begin(); iter != node.End(); ++iter )
			ProcessPropNode( prop, *iter );
	}

	// process fundamental types and String
	template< typename T >
	void ProcessPropNode( const PropNode& prop, T& var, typename std::enable_if< std::is_fundamental< T >::value || std::is_same< T, String >::value >::type* = 0  )
	{
		var = prop.Get< T >();
	}

	// process fundamental types and String
	template< typename T >
	std::unique_ptr< T > CreateFromPropNode( const PropNode& prop )
	{
		std::unique_ptr< T > var( GetFactory().Create< T >( prop.GetStr( "type" ) ) );
		ProcessPropNode( prop, *var );
		return var;
	}

	// convenience macro that automatically derives name from variable name
	#define PROCESS_PROPERTY( _prop_, _var_, _default_ ) ProcessPropNode( _prop_, _var_, GetCleanVarName( #_var_ ), _default_ )
	#define PROCESS_PROPERTY_NAMED( _prop_, _var_, _name_, _default_ ) ProcessPropNode( _prop_, _var_, _name_, _default_ )
}

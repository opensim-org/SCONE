#pragma once

#include "core.h"
#include "Exception.h"
#include <vector>
#include <flut/string_tools.hpp>

namespace scone
{
	class HasName
	{
	public:
		virtual ~HasName() = 0;
		virtual const String& GetName() const = 0;
	};
	inline HasName::~HasName() {} // must provide implementation for destructor of pure virtual class

	template< typename T >
	T& FindByName( std::vector< T >& cont, const String& name )
	{
		using flut::quoted;
		auto it = std::find_if( cont.begin(), cont.end(), [&]( T& item ) { return item->GetName() == name; } );
		SCONE_THROW_IF( it == cont.end(), "Could not find " + quoted( name ) );
		return *it;
	}

	template< typename T >
	bool HasElementWithName( std::vector< T >& cont, const String& name )
	{
		return cont.end() != std::find_if( cont.begin(), cont.end(), [&]( T& item ) { return item->GetName() == name; } );
	}

	// TODO: move to elsewhere
	template< typename T >
	Index FindIndex( const std::vector< T > cont, const T& item )
	{
		auto it = std::find( cont.begin(), cont.end(), item );
		return it != cont.end() ? static_cast< Index >( it - cont.begin() ) : NoIndex;
	}

	// TODO: move to elsewhere
	template< typename T >
	Index FindIndexOrThrow( const std::vector< T > cont, const T& item )
	{
		auto it = std::find( cont.begin(), cont.end(), item );
		SCONE_THROW_IF( it == cont.end(), "Could not find " + to_str( item ) );
		return static_cast< Index >( it - cont.begin() );
	}
}

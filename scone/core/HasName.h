#pragma once

#include "core.h"
#include "Exception.h"
#include "../sim/Types.h"

namespace scone
{
	class CORE_API HasName
	{
	public:
		HasName();
		virtual ~HasName();
		virtual const String& GetName() const = 0;
	};

	template< typename T >
	T& FindByName( std::vector< T >& cont, const String& name )
	{
		auto it = std::find_if( cont.begin(), cont.end(), [&]( T& item ) { return item->GetName() == name; } );
		SCONE_THROW_IF( it == cont.end(), "Could not find " + GetQuoted( name ) );
		return *it;
	}

	template< typename T >
	bool HasElementWithName( std::vector< T >& cont, const String& name )
	{
		return cont.end() != std::find_if( cont.begin(), cont.end(), [&]( T& item ) { return item->GetName() == name; } );
	}

	template< typename T >
	T& FindBySide( std::vector< T >& cont, Side side )
	{
		auto it = std::find_if( cont.begin(), cont.end(), [&]( T& item ) { return item->GetSide() == side; } );
		SCONE_THROW_IF( it == cont.end(), "Could not find item with side " + ToString( side ) );
		return *it;
	}
}

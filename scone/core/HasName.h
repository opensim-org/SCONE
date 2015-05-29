#pragma once

#include "core.h"
#include "Exception.h"

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
		if ( it == cont.end() )
			SCONE_THROW( "Could not find " + GetQuoted( name ) );
		return *it;
	}

	template< typename T >
	bool HasElementWithName( std::vector< T >& cont, const String& name )
	{
		return cont.end() != std::find_if( cont.begin(), cont.end(), [&]( T& item ) { return item->GetName() == name; } );
	}
}

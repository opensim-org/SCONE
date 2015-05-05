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
	T& FindNamed( std::vector< std::unique_ptr< T > >& cont, const String& name )
	{
		auto it = std::find_if( cont.begin(), cont.end(), [&]( std::unique_ptr< T >& item ) { return item->GetName() == name; } );
		if ( it == cont.end() )
			SCONE_THROW( "Could not find " + GetQuoted( name ) );

		return **it;
	}
}

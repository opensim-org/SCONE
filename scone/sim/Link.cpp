#include "stdafx.h"
#include "Link.h"

#include "Body.h"
#include "Joint.h"

namespace scone
{
	namespace sim
	{
		String Link::ToString( const String& prefix )
		{
			String s = prefix + body().GetName() + ( has_joint() ? ( " (" + joint().GetName() + ")\n" ) : "\n" );
			for ( auto it = m_Children.begin(); it != m_Children.end(); ++it )
				s += (*it)->ToString( prefix + "  " );

			return s;
		}

	}
}

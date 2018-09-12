#include "Objective.h"
#include "scone/core/Exception.h"

namespace scone
{
	Objective::Objective( const PropNode& props ) :
	HasSignature( props )
	{
		if ( auto p = props.try_get_child( "Parameters" ) )
		{
			for ( auto& par : *p )
				info().add( ParInfo( par.first, par.second ) );
		}
	}

	Objective::~Objective()
	{}
}

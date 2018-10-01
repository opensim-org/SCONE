/*
** Objective.cpp
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

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

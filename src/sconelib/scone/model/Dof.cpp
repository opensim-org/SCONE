/*
** Dof.cpp
**
** Copyright (C) 2013-2021 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "Dof.h"
#include "Model.h"
#include "Muscle.h"

#pragma warning( disable: 4355 )

namespace scone
{
	Dof::Dof( const Joint* j ) : m_Joint( j ) {}
	Dof::~Dof() {}

	Real Dof::GetMuscleMoment() const
	{
		Real mom = 0.0;
		for ( const auto& mus : GetModel().GetMuscles() )
		{
			if ( mus->HasMomentArm( *this ) )
				mom += mus->GetMoment( *this );
		}
		return mom;
	}
}

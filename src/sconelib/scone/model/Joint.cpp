/*
** Joint.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "Joint.h"
#include "Body.h"
#include "Model.h"
#include "Dof.h"

namespace scone
{
	Joint::Joint( const Body& body, Joint* parent ) :
	m_Body( body ),
	m_pParent( parent )
	{}

	Real Joint::GetLoad() const
	{
		return xo::length( GetReactionForce() ) / m_Body.GetModel().GetBW();
	}

	const std::vector< Dof* >& Joint::GetDofs() const
	{
		if ( m_Dofs.empty() )
		{
			for ( auto& dof : m_Body.GetModel().GetDofs() )
				if ( this == &dof->GetJoint() )
					m_Dofs.push_back( dof.get() );
		}
		return m_Dofs;
	}

	Joint::~Joint()
	{}
}

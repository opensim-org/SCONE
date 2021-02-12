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
	Joint::Joint( Body& body, Body& parent_body ) :
	m_Body( body ),
	m_ParentBody( parent_body )
	{
		m_Body.m_Joint = this;
	}

	Real Joint::GetLimitMoment() const
	{
		Real force{ 0 };
		for ( const auto& dof : GetDofs() )
			force += dof->GetLimitMoment();
		return force;
	}

	Real Joint::GetLoad() const
	{
		return xo::length( GetReactionForce() ) / m_Body.GetModel().GetBW();
	}

	const std::vector< Dof* >& Joint::GetDofs() const
	{
		if ( m_Dofs.empty() )
		{
			for ( auto& dof : m_Body.GetModel().GetDofs() )
				if ( this == dof->GetJoint() )
					m_Dofs.push_back( dof.get() );
		}
		return m_Dofs;
	}

	void Joint::StoreData( Storage<Real>::Frame& frame, const StoreDataFlags& flags ) const
	{
		// store joint reaction force magnitude
		if ( flags( StoreDataTypes::JointReactionForce ) )
			frame[ GetName() + ".load" ] = GetLoad();
	}

	PropNode Joint::GetInfo() const
	{
		PropNode pn;
		for ( auto& d : GetDofs() )
			pn[ d->GetName() ] = d->GetInfo();
		return pn;
	}

	Joint::~Joint()
	{}
}

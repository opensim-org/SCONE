#include "stdafx.h"
#include "Muscle.h"

#include "Link.h"
#include "Joint.h"
#include "Dof.h"

#pragma warning( disable: 4355 )

namespace scone
{
	namespace sim
	{
		Muscle::Muscle() :
		Actuator()
		{
		}
		
		Muscle::~Muscle()
		{
		}

		bool Muscle::HasMomentArm( const Dof& dof ) const
		{
			const sim::Link& orgLink = GetOriginLink();
			const sim::Link& insLink = GetInsertionLink();
			const sim::Link* l = &insLink;
			while ( l && l != &orgLink )
			{
				if ( l->GetJoint().HasDof( dof.GetName() ) )
					return true;

				l = &l->GetParent();
			}
			return false;
		}

		void Muscle::StoreData( Storage< Real >::Frame& frame )
		{
			//frame[ GetName() + ".length" ] = GetLength();
			//frame[ GetName() + ".activation" ] = GetActivation();
			//frame[ GetName() + ".force" ] = GetFiberForce();
		}
	}
}

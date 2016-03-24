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

		scone::Count Muscle::GetJointCount() const
		{
			Count joint_count = 0;
			const sim::Link* orgLink = &GetOriginLink();
			const sim::Link* insLink = &GetInsertionLink();
			for ( const Link* l = orgLink; l && l != insLink; l = &l->GetParent() )
				++joint_count;
			return joint_count;
		}

		void Muscle::StoreData( Storage< Real >::Frame& frame )
		{
			//frame[ GetName() + ".length" ] = GetLength();
			//frame[ GetName() + ".activation" ] = GetActivation();
			//frame[ GetName() + ".force" ] = GetFiberForce();
		}
	}
}

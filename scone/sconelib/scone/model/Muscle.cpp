#include "Muscle.h"

#include "Link.h"
#include "Joint.h"
#include "Dof.h"

#pragma warning( disable: 4355 )

namespace scone
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
		const Link& orgLink = GetOriginLink();
		const Link& insLink = GetInsertionLink();
		const Link* l = &insLink;
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
		const Link* orgLink = &GetOriginLink();
		const Link* insLink = &GetInsertionLink();
		for ( const Link* l = orgLink; l && l != insLink; l = &l->GetParent() )
			++joint_count;
		return joint_count;
	}

	void Muscle::StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags )
	{
		if ( flags( StoreDataTypes::MuscleExcitation ) )
			frame[ GetName() + ".excitation" ] = GetInput();

		if ( flags( StoreDataTypes::MuscleActivation ) && !flags( StoreDataTypes::State ) )
			frame[ GetName() + ".activation" ] = GetActivation();

		if ( flags( StoreDataTypes::MuscleFiberProperties ) )
		{
			frame[ GetName() + ".fiber_length" ] = GetNormalizedFiberLength();
			frame[ GetName() + ".fiber_velocity" ] = GetNormalizedFiberVelocity();
			frame[ GetName() + ".fiber_force" ] = GetNormalizedFiberForce();
		}
	}
}

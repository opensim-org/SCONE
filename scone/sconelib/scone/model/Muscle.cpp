#include "Muscle.h"

#include "Link.h"
#include "Joint.h"
#include "Dof.h"
#include "flut/math/math.hpp"
#include "Body.h"
#include "Model.h"
#include "../core/math.h"

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

	Real Muscle::GetNormalizedSpindleRate() const
	{
		// derived from [Prochazka1999], velocity component normalized to unit length
		double vel = ( 65.0 / 200.0 ) * flut::math::signed_sqrt( GetNormalizedFiberVelocity() );
		double disp = GetNormalizedFiberLength();
		return std::max( 0.0, vel + disp );
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

	bool Muscle::IsAntagonist( const Muscle& other ) const
	{
		// TODO: more efficient
		for ( auto& dof : GetOriginLink().GetBody().GetModel().GetDofs() )
		{
			if ( HasMomentArm( *dof ) && other.HasMomentArm( *dof ) )
			{
				if ( Sign( GetMomentArm( *dof ) ) != Sign( other.GetMomentArm( *dof ) ) )
					return true;
			}
		}
		return false;
	}

	bool Muscle::HasSharedDofs( const Muscle& other ) const
	{
		// TODO: more efficient
		for ( auto& dof : GetOriginLink().GetBody().GetModel().GetDofs() )
		{
			if ( HasMomentArm( *dof ) && other.HasMomentArm( *dof ) )
				return true;
		}
		return false;
	}

	void Muscle::StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const
	{
		if ( flags( StoreDataTypes::MuscleExcitation ) )
			frame[ GetName() + ".excitation" ] = flut::math::clamped( GetInput(), 0.0, 1.0 );

		if ( flags( StoreDataTypes::MuscleActivation ) && !flags( StoreDataTypes::State ) )
			frame[ GetName() + ".activation" ] = GetActivation();

		if ( flags( StoreDataTypes::MuscleFiberProperties ) )
		{
			frame[ GetName() + ".F" ] = GetNormalizedFiberForce();
			frame[ GetName() + ".L" ] = GetNormalizedFiberLength();
			frame[ GetName() + ".V" ] = GetNormalizedFiberVelocity();
			frame[ GetName() + ".S" ] = GetNormalizedSpindleRate();
		}
	}
}

#include "Muscle.h"

#include "Link.h"
#include "Joint.h"
#include "Dof.h"
#include "flut/math/math.hpp"
#include "Body.h"
#include "Model.h"
#include "../core/math.h"
#include "../core/Profiler.h"

#pragma warning( disable: 4355 )

namespace scone
{
	Muscle::Muscle() : Actuator()
	{}

	Muscle::~Muscle()
	{}

	Real Muscle::GetNormalizedSpindleRate() const
	{
		// derived from [Prochazka1999], velocity component normalized to unit length
		double vel = ( 65.0 / 200.0 ) * flut::math::signed_sqrt( GetNormalizedFiberVelocity() );
		double disp = GetNormalizedFiberLength();
		return std::max( 0.0, vel + disp );
	}

	bool Muscle::HasMomentArm( const Dof& dof ) const
	{
		SCONE_PROFILE_FUNCTION;

		return GetMomentArm( dof ) != 0;
	}

	scone::Count Muscle::GetJointCount() const
	{
		SCONE_PROFILE_FUNCTION;

		Count joint_count = 0;
		const Link* orgLink = &GetOriginLink();
		const Link* insLink = &GetInsertionLink();
		for ( const Link* l = orgLink; l && l != insLink; l = &l->GetParent() )
			++joint_count;
		return joint_count;
	}

	bool Muscle::IsAntagonist( const Muscle& other ) const
	{
		SCONE_PROFILE_FUNCTION;

		for ( auto& dof : GetModel().GetDofs() )
		{
			auto mom1 = GetMomentArm( *dof );
			auto mom2 = other.GetMomentArm( *dof );
			if ( mom1 != 0 && mom2 != 0 && Sign( mom1 ) != Sign( mom2 ) )
				return true;
		}
		return false;
	}

	bool Muscle::IsAgonist( const Muscle& other ) const
	{
		for ( auto& dof : GetModel().GetDofs() )
		{
			auto mom1 = GetMomentArm( *dof );
			auto mom2 = other.GetMomentArm( *dof );
			if ( mom1 != 0 && mom2 != 0 && Sign( mom1 ) == Sign( mom2 ) )
				return true;
		}
		return false;
	}

	bool Muscle::HasSharedDofs( const Muscle& other ) const
	{
		SCONE_PROFILE_FUNCTION;

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

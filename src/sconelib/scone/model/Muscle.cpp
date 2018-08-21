#include "Muscle.h"

#include "Link.h"
#include "Joint.h"
#include "Dof.h"
#include "xo/numerical/math.h"
#include "Body.h"
#include "Model.h"
#include "scone/core/math.h"
#include "scone/core/Profiler.h"

#pragma warning( disable: 4355 )

namespace scone
{
	Muscle::Muscle() : Actuator()
	{}

	Muscle::~Muscle()
	{}

	scone::Real Muscle::GetNormalizedMomentArm( const Dof& dof ) const
	{
		Real mom = GetMomentArm( dof );
		if ( mom != 0 )
		{
			// normalize
			Real total_mom = 0.0;
			for ( auto& d : GetModel().GetDofs() )
				total_mom += abs( GetMomentArm( *d ) );
			return mom / total_mom;
		}
		else return mom;
	}

	Real Muscle::GetNormalizedSpindleRate() const
	{
		// derived from [Prochazka1999], velocity component normalized to unit length
		double vel = ( 65.0 / 200.0 ) * xo::signed_sqrt( GetNormalizedFiberVelocity() );
		double disp = GetNormalizedFiberLength();
		return std::max( 0.0, vel + disp );
	}

	scone::Side Muscle::GetSide() const
	{
		return GetSideFromName( GetName() );
	}

	bool Muscle::HasMomentArm( const Dof& dof ) const
	{
		SCONE_PROFILE_FUNCTION;

		return GetMomentArm( dof ) != 0;
	}

	const std::vector< const Joint* >& Muscle::GetJoints() const
	{
		SCONE_PROFILE_FUNCTION;

		if ( m_Joints.empty() )
		{
			const Link* orgLink = &GetOriginLink();
			const Link* insLink = &GetInsertionLink();
			for ( const Link* l = insLink; l && l != orgLink; l = &l->GetParent() )
				m_Joints.push_back( &l->GetJoint() );
		}

		return m_Joints;
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

		for ( auto& dof : GetOriginLink().GetBody().GetModel().GetDofs() )
		{
			if ( HasMomentArm( *dof ) && other.HasMomentArm( *dof ) )
				return true;
		}
		return false;
	}

	bool Muscle::HasSharedBodies( const Muscle& other ) const
	{
		return &GetOriginLink() == &other.GetOriginLink()
			|| &GetOriginLink() == &other.GetInsertionLink()
			|| &GetInsertionLink() == &other.GetOriginLink()
			|| &GetInsertionLink() == &other.GetInsertionLink();
	}

	void Muscle::StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const
	{
		if ( flags( StoreDataTypes::MuscleExcitation ) )
			frame[ GetName() + ".input" ] = GetInput();

		if ( flags( StoreDataTypes::MuscleExcitation ) )
			frame[ GetName() + ".excitation" ] = GetExcitation();

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

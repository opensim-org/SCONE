/*
** Muscle.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "Muscle.h"

#include "Joint.h"
#include "Dof.h"
#include "xo/numerical/math.h"
#include "Body.h"
#include "Model.h"
#include "scone/core/math.h"
#include "scone/core/profiler_config.h"

#pragma warning( disable: 4355 )

namespace scone
{
	Muscle::Muscle() : Actuator()
	{}

	Muscle::~Muscle()
	{}

	Real Muscle::GetNormalizedMomentArm( const Dof& dof ) const
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

	Real Muscle::GetMass( Real specific_tension, Real muscle_density ) const
	{
		// from OpenSim Umberger metabolic energy model docs
		return ( GetMaxIsometricForce() / specific_tension ) * muscle_density * GetOptimalFiberLength();
	}

	Real Muscle::GetPCSA( Real specific_tension ) const
	{
		return GetMaxIsometricForce() / specific_tension;
	}

	Real Muscle::GetNormalizedSpindleRate() const
	{
		// derived from [Prochazka1999], velocity component normalized to unit length
		double vel = ( 65.0 / 200.0 ) * xo::signed_sqrt( GetNormalizedFiberVelocity() );
		double disp = GetNormalizedFiberLength();
		return std::max( 0.0, vel + disp );
	}

	Side Muscle::GetSide() const
	{
		return GetSideFromName( GetName() );
	}

	bool Muscle::HasMomentArm( const Dof& dof ) const
	{
		return GetMomentArm( dof ) != 0;
	}

	const std::vector< const Joint* >& Muscle::GetJoints() const
	{
		SCONE_PROFILE_FUNCTION;

		if ( m_Joints.empty() )
		{
			const Body* orgBody = &GetOriginBody();
			const Body* insBody = &GetInsertionBody();
			for ( const Body* b = insBody; b && b != orgBody; b = b->GetParentBody() )
				m_Joints.push_back( b->GetJoint() );
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

		for ( auto& dof : GetOriginBody().GetModel().GetDofs() )
		{
			if ( HasMomentArm( *dof ) && other.HasMomentArm( *dof ) )
				return true;
		}
		return false;
	}

	bool Muscle::HasSharedBodies( const Muscle& other ) const
	{
		return &GetOriginBody() == &other.GetOriginBody()
			|| &GetOriginBody() == &other.GetInsertionBody()
			|| &GetInsertionBody() == &other.GetOriginBody()
			|| &GetInsertionBody() == &other.GetInsertionBody();
	}

	void Muscle::StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const
	{
		Actuator::StoreData( frame, flags );

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

/*
** Muscle.cpp
**
** Copyright (C) 2013-2021 Thomas Geijtenbeek and contributors. All rights reserved.
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

	Real Muscle::GetMoment( const Dof& dof ) const
	{
		return GetForce() * GetMomentArm( dof );
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
		for ( const auto& d : m_Dofs )
			if ( d == &dof )
				return true;
		return false;
	}

	const std::vector< const Joint* >& Muscle::GetJoints() const
	{
		SCONE_ASSERT( !m_Joints.empty() ); // Initialize in derived class via InitJointsDofs()
		return m_Joints;
	}

	const std::vector<const Dof*>& Muscle::GetDofs() const
	{
		SCONE_ASSERT( !m_Dofs.empty() ); // Initialize in derived class via InitJointsDofs()
		return m_Dofs;
	}

	bool Muscle::IsAntagonist( const Muscle& other ) const
	{
		for ( auto& dof : GetModel().GetDofs() )
		{
			if ( HasMomentArm( *dof ) && other.HasMomentArm( *dof ) )
				if ( Sign( GetMomentArm( *dof ) ) != Sign( other.GetMomentArm( *dof ) ) )
					return true;
		}
		return false;
	}

	bool Muscle::IsAgonist( const Muscle& other ) const
	{
		for ( auto& dof : GetModel().GetDofs() )
		{
			if ( HasMomentArm( *dof ) && other.HasMomentArm( *dof ) )
				if ( Sign( GetMomentArm( *dof ) ) == Sign( other.GetMomentArm( *dof ) ) )
					return true;
		}
		return false;
	}

	bool Muscle::HasSharedDofs( const Muscle& other ) const
	{
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

	bool Muscle::HasSharedJoints( const Muscle& other ) const
	{
		const Body* org1 = &GetOriginBody();
		const Body* org2 = &other.GetOriginBody();
		for ( const Body* b1 = &GetInsertionBody(); b1 != org1; b1 = b1->GetParentBody() )
			for ( const Body* b2 = &other.GetInsertionBody(); b2 != org2; b2 = b2->GetParentBody() )
				if ( b1->GetJoint() == b2->GetJoint() )
					return true;
		return false;
	}

	void Muscle::StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const
	{
		Actuator::StoreData( frame, flags );

		if ( flags( StoreDataTypes::MuscleProperties) )
		{
			frame[ GetName() + ".excitation" ] = GetExcitation();
			if ( !flags( StoreDataTypes::State ) ) // activation is also part of state
				frame[ GetName() + ".activation" ] = GetActivation();

			// tendon / mtu properties
			frame[ GetName() + ".tendon_length" ] = GetTendonLength();
			frame[ GetName() + ".tendon_length_norm" ] = GetNormalizedTendonLength() - 1;
			frame[ GetName() + ".mtu_length" ] = GetLength();
			frame[ GetName() + ".mtu_velocity" ] = GetVelocity();
			frame[ GetName() + ".mtu_force" ] = GetForce();
			frame[ GetName() + ".mtu_force_norm" ] = GetNormalizedForce();
			frame[ GetName() + ".mtu_power" ] = GetForce() * GetVelocity();

			// fiber properties
			frame[ GetName() + ".cos_pennation_angle" ] = GetCosPennationAngle();
			frame[ GetName() + ".force_length_multiplier" ] = GetActiveForceLengthMultipler();
			frame[ GetName() + ".passive_fiber_force_norm" ] = GetPassiveFiberForce() / GetMaxIsometricForce();
			frame[ GetName() + ".fiber_length_norm" ] = GetNormalizedFiberLength();
			frame[ GetName() + ".fiber_velocity_norm" ] = GetNormalizedFiberVelocity();
		}

		if ( flags( StoreDataTypes::MuscleDofMomentPower ) )
		{
			for ( auto& d : GetDofs() )
			{
				auto name = GetName() + "." + d->GetName();
				auto ma = GetMomentArm( *d );
				auto mom = GetForce() * ma;
				frame[ name + ".moment_arm" ] = ma;
				frame[ name + ".moment" ] = mom;
				frame[ name + ".power" ] = mom * d->GetVel();
			}
		}
	}

	PropNode Muscle::GetInfo() const
	{
		PropNode pn;
		pn[ "max_isometric_force" ] = GetMaxIsometricForce();
		pn[ "optimal_fiber_length" ] = GetOptimalFiberLength();
		pn[ "tendon_slack_length" ] = GetTendonSlackLength();
		return pn;
	}

	void Muscle::InitJointsDofs()
	{
		SCONE_ASSERT( m_Joints.empty() && m_Dofs.empty() );
		const Body* orgBody = &GetOriginBody();
		const Body* insBody = &GetInsertionBody();
		for ( const Body* b = insBody; b && b != orgBody; b = b->GetParentBody() )
		{
			auto* j = b->GetJoint();
			m_Joints.push_back( j );
			xo::append( m_Dofs, j->GetDofs() );
		}
	}
}

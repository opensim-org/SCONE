/*
** Sensors.cpp
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "Sensors.h"
#include "Model.h"
#include "Muscle.h"
#include "Location.h"
#include "Body.h"
#include "Dof.h"
#include "scone/core/string_tools.h"
#include "scone/core/Log.h"

namespace scone
{
	Real MuscleForceSensor::GetValue() const { return m_Muscle.GetNormalizedForce(); }
	String MuscleForceSensor::GetName() const { return m_Muscle.GetName() + ".F"; }

	Real MuscleLengthSensor::GetValue() const { return m_Muscle.GetNormalizedFiberLength(); }
	String MuscleLengthSensor::GetName() const { return m_Muscle.GetName() + ".L"; }

	Real MuscleVelocitySensor::GetValue() const { return m_Muscle.GetNormalizedFiberVelocity(); }
	String MuscleVelocitySensor::GetName() const { return m_Muscle.GetName() + ".V"; }

	Real MuscleSpindleSensor::GetValue() const { return m_Muscle.GetNormalizedSpindleRate(); }
	String MuscleSpindleSensor::GetName() const { return m_Muscle.GetName() + ".S"; }

	Real MuscleExcitationSensor::GetValue() const { return m_Muscle.GetExcitation(); }
	String MuscleExcitationSensor::GetName() const { return m_Muscle.GetName() + ".excitation"; }

	Real DofPositionSensor::GetValue() const
	{
		// TODO: get rid of this if statement and use a "constant" Dof?
		if ( m_pRootDof )
			return m_pRootDof->GetPos() + m_Dof.GetPos();
		else return m_Dof.GetPos();
	}

	String DofPositionSensor::GetName() const
	{
		return m_Dof.GetName() + ".DP";
	}

	Real DofVelocitySensor::GetValue() const
	{
		// TODO: get rid of this if statement and use a "constant" Dof?
		if ( m_pRootDof )
			return m_pRootDof->GetVel() + m_Dof.GetVel();
		else return m_Dof.GetVel();
	}

	String DofVelocitySensor::GetName() const
	{
		return m_Dof.GetName() + ".DV";
	}

	Real DofPosVelSensor::GetValue() const
	{
		// TODO: get rid of this if statement and use a "constant" Dof?
		if ( m_pRootDof )
			return m_pRootDof->GetPos() + m_Dof.GetPos() + m_KV * ( m_pRootDof->GetVel() + m_Dof.GetVel() );
		else return m_Dof.GetPos() + m_KV * m_Dof.GetVel();
	}

	String DofPosVelSensor::GetName() const
	{
		return m_Dof.GetName() + ".DPV";
	}

	Real LegLoadSensor::GetValue() const
	{
		return m_Leg.GetLoad();
	}

	String LegLoadSensor::GetName() const
	{
		return m_Leg.GetName() + ".LD";
	}

	const char* g_BodyChannelNames[] = { "X", "Y", "Z" };

	scone::Real BodyOriSensor::GetValue( index_t idx ) const
	{
		return xo::rotation_vector_from_quat( m_Body.GetOrientation() )[ idx ];
	}

	scone::String BodyOriSensor::GetName() const
	{
		return m_Body.GetName() + ".Ori";
	}

	scone::Real BodyAngVelSensor::GetValue( index_t idx ) const
	{
		return m_Body.GetAngVel()[ idx ];
	}

	scone::String BodyAngVelSensor::GetName() const
	{
		return m_Body.GetName() + ".AngVel";
	}
}

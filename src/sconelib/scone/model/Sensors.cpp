/*
** Sensors.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "Sensors.h"
#include "Model.h"
#include "Muscle.h"
#include "Body.h"
#include "Dof.h"

namespace scone
{
	Real DofPositionSensor::GetValue() const { return root_dof_ ? root_dof_->GetPos() + dof_.GetPos() : dof_.GetPos(); }
	Real DofVelocitySensor::GetValue() const { return root_dof_ ? root_dof_->GetVel() + dof_.GetVel() : dof_.GetVel(); }
	Real DofPosVelSensor::GetValue() const {
		if ( root_dof_ )
			return root_dof_->GetPos() + dof_.GetPos() + kv_ * ( root_dof_->GetVel() + dof_.GetVel() );
		else return dof_.GetPos() + kv_ * dof_.GetVel();
	}

	String BodyPointPositionSensor::GetName() const { return body_.GetName() + ".PP"; }
	Real BodyPointPositionSensor::GetValue() const {
		return xo::dot_product( direction_, body_.GetPosOfPointOnBody( offset_ ) );
	}

	String BodyPointVelocitySensor::GetName() const { return body_.GetName() + ".PV"; }
	Real BodyPointVelocitySensor::GetValue() const {
		return xo::dot_product( direction_, body_.GetLinVelOfPointOnBody( offset_ ) );
	}

	String BodyPointAccelerationSensor::GetName() const { return body_.GetName() + ".PA"; }
	Real BodyPointAccelerationSensor::GetValue() const {
		return xo::dot_product( direction_, body_.GetLinAccOfPointOnBody( offset_ ) );
	}
}

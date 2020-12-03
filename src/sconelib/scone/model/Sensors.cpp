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
#include "xo/geometry/vec3.h"
#include "xo/geometry/quat.h"
#include "xo/container/container_algorithms.h"
#include <numeric>

namespace scone
{
	String MuscleForceSensor::GetName() const { return muscle_.GetName() + ".F"; }
	Real MuscleForceSensor::GetValue() const { return muscle_.GetNormalizedForce(); }

	String MuscleLengthSensor::GetName() const { return muscle_.GetName() + ".L"; }
	Real MuscleLengthSensor::GetValue() const { return muscle_.GetNormalizedFiberLength(); }

	String MuscleVelocitySensor::GetName() const { return muscle_.GetName() + ".V"; }
	Real MuscleVelocitySensor::GetValue() const { return muscle_.GetNormalizedFiberVelocity(); }

	String MuscleLengthVelocitySensor::GetName() const { return muscle_.GetName() + ".L"; }
	Real MuscleLengthVelocitySensor::GetValue() const { return muscle_.GetNormalizedFiberLength() + kv_ * muscle_.GetNormalizedFiberVelocity() ; }

	String MuscleLengthVelocitySqrtSensor::GetName() const { return muscle_.GetName() + ".L"; }
	Real MuscleLengthVelocitySqrtSensor::GetValue() const { return muscle_.GetNormalizedFiberLength() + kv_ * xo::signed_sqrt( muscle_.GetNormalizedFiberVelocity() ); }

	String MuscleSpindleSensor::GetName() const { return muscle_.GetName() + ".S"; }
	Real MuscleSpindleSensor::GetValue() const { return muscle_.GetNormalizedSpindleRate(); }

	String MuscleSpindleSensor2::GetName() const { return muscle_.GetName() + ".L"; }
	Real MuscleSpindleSensor2::GetValue() const {
		auto l = muscle_.GetNormalizedFiberLength() - l0_;
		auto v = kv_ * xo::signed_sqrt( muscle_.GetNormalizedFiberVelocity() );
		return std::max( 0.0, l + v );
	}

	String MuscleExcitationSensor::GetName() const { return muscle_.GetName() + ".excitation"; }
	Real MuscleExcitationSensor::GetValue() const { return muscle_.GetExcitation(); }

	String MuscleActivationSensor::GetName() const { return muscle_.GetName() + ".A"; }
	Real MuscleActivationSensor::GetValue() const { return muscle_.GetActivation(); }

	String LegLoadSensor::GetName() const { return leg_.GetName() + ".LD"; }
	Real LegLoadSensor::GetValue() const { return leg_.GetLoad(); }

	String DofPositionSensor::GetName() const { return dof_.GetName() + ".DP"; }
	Real DofPositionSensor::GetValue() const { return root_dof_ ? root_dof_->GetPos() + dof_.GetPos() : dof_.GetPos(); }

	String DofVelocitySensor::GetName() const { return dof_.GetName() + ".DV"; }
	Real DofVelocitySensor::GetValue() const { return root_dof_ ? root_dof_->GetVel() + dof_.GetVel() : dof_.GetVel(); }

	String DofPosVelSensor::GetName() const { return GetSidedName( dof_.GetName(), side_ ) + ".DPV"; }
	Real DofPosVelSensor::GetValue() const {
		Real value = root_dof_ ? root_dof_->GetPos() + dof_.GetPos() + kv_ * ( root_dof_->GetVel() + dof_.GetVel() ) :
			dof_.GetPos() + kv_ * dof_.GetVel();
		return side_ == RightSide ? -value : value; // mirror for right side, see SensorNeuron.cpp
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

	BodyOrientationSensor::BodyOrientationSensor( const Body& body, const Vec3& dir, const String& postfix, Side side ) :
		body_( body ), dir_( GetSidedAxis( dir, side ) ), name_( GetSidedName( body_.GetName() + postfix, side ) + ".BO" ) {}
	Real BodyOrientationSensor::GetValue() const {
		return xo::dot_product( body_.GetOrientation() * dir_, xo::rotation_vector_from_quat( xo::normalized_fast( body_.GetOrientation() ) ) );
	}

	BodyAngularVelocitySensor::BodyAngularVelocitySensor( const Body& body, const Vec3& dir, const String& postfix, Side side ) :
		body_( body ), dir_( GetSidedAxis( dir, side ) ), name_( GetSidedName( body_.GetName() + postfix, side ) + ".BAV" ) {}
	Real BodyAngularVelocitySensor::GetValue() const {
		return xo::dot_product( body_.GetOrientation() * dir_, body_.GetAngVel() );
	}

	BodyOriVelSensor::BodyOriVelSensor( const Body& body, const Vec3& dir, double kv, const String& postfix, Side side, double target ) :
		body_( body ), dir_( GetSidedAxis( dir, side ) ), kv_( kv ), name_( GetSidedName( body_.GetName() + postfix, side ) + ".BOV" ), target_( target ) {}
	Real BodyOriVelSensor::GetValue() const {
		auto ori_rv = xo::rotation_vector_from_quat( xo::normalized( body_.GetOrientation() ) );
		auto dir = body_.GetOrientation() * dir_;
		return xo::dot_product( dir, ori_rv ) + kv_ * xo::dot_product( dir, body_.GetAngVel() ) - target_;
	}

	ComBosSensor::ComBosSensor( const Model& mod, const Vec3& dir, double kv, const String& name, Side side ) :
		model_( mod ), dir_( GetSidedDirection( dir, side ) ), kv_( kv ), name_( GetSidedName( name, side ) + ".CB" ) {
		SCONE_ERROR_IF( model_.GetLegCount() <= 0, "Could not find legs in model" );
		SCONE_ERROR_IF( model_.GetRootBody() == nullptr, "Model has no root body" );
	}
	Real ComBosSensor::GetValue() const {
		const auto com = model_.GetProjectedOntoGround( model_.GetComPos() + kv_ * model_.GetComVel() );
		const auto bosp = xo::average( model_.GetLegs(), Vec3(),
			[]( const Vec3& v, const LegUP& l ) { return v + l->GetFootBody().GetComPos(); } );
		const auto bosv = xo::average( model_.GetLegs(), Vec3(),
			[]( const Vec3& v, const LegUP& l ) { return v + l->GetFootBody().GetComVel(); } );
		const auto bos = model_.GetProjectedOntoGround( bosp + kv_ * bosv );
		return xo::dot_product( model_.GetRootBody()->GetOrientation() * dir_, com - bos );
	}

	ModulatedSensor::ModulatedSensor( const Sensor& sensor, const Sensor& modulator, double gain, double ofs, const String& name, xo::boundsd mod_range ) :
		sensor_( sensor ), modulator_( modulator ), gain_( gain ), ofs_( ofs ), name_( name ), mod_range_( mod_range ) {}
	Real ModulatedSensor::GetValue() const {
		auto mv = mod_range_.clamped( gain_ * modulator_.GetValue() + ofs_ );
		return sensor_.GetValue() * mv;
	}
}

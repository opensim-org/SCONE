#include "stdafx.h"
#include "Sensors.h"
#include "Model.h"
#include "Muscle.h"
#include "Area.h"
#include "Body.h"
#include "../cs/Tools.h"

namespace scone
{
	namespace sim
	{
		MuscleSensor::MuscleSensor( const PropNode& pn, opt::ParamSet& par, sim::Model& model, const Area& target_area ) :
		Sensor( pn, par, model, target_area ),
		m_Muscle( *FindByName( model.GetMuscles(), pn.GetStr( "muscle" ) ) )
		{
		}

		scone::Real MuscleForceSensor::GetValue() const 
		{
			return m_Muscle.GetNormalizedForce();
		}

		scone::String MuscleForceSensor::GetName() const 
		{
			return m_Muscle.GetName() + ".F";
		}

		scone::Real MuscleLengthSensor::GetValue() const 
		{
			return m_Muscle.GetNormalizedFiberLength();
		}

		scone::String MuscleLengthSensor::GetName() const 
		{
			return m_Muscle.GetName() + ".L";
		}

		scone::Real MuscleVelocitySensor::GetValue() const 
		{
			return m_Muscle.GetNormalizedFiberVelocity();
		}

		scone::String MuscleVelocitySensor::GetName() const 
		{
			return m_Muscle.GetName() + ".V";
		}

		DofSensor::DofSensor( const PropNode& pn, opt::ParamSet& par, sim::Model& model, const Area& target_area ) :
		Sensor( pn, par, model, target_area ),
		m_Dof( *FindByName( model.GetDofs(), pn.GetStr( "dof" ) ) )
		{
		}

		scone::Real DofPositionSensor::GetValue() const 
		{
			return m_Dof.GetPos();
		}

		scone::String DofPositionSensor::GetName() const 
		{
			return m_Dof.GetName() + ".P";
		}

		scone::Real DofVelocitySensor::GetValue() const 
		{
			return m_Dof.GetVel();
		}

		scone::String DofVelocitySensor::GetName() const 
		{
			return m_Dof.GetName() + ".V";
		}

		LegLoadSensor::LegLoadSensor( const PropNode& pn, opt::ParamSet& par, sim::Model& model, const Area& target_area ) :
		Sensor( pn, par, model, target_area ),
		m_Leg( *FindBySide( model.GetLegs(), target_area.side ) )
		{
		}

		scone::Real LegLoadSensor::GetValue() const 
		{
			return m_Leg.GetLoad();
		}

		scone::String LegLoadSensor::GetName() const 
		{
			return "Load." + m_Leg.GetName();
		}

		SagittalPostureSensor::SagittalPostureSensor( const PropNode& pn, opt::ParamSet& par, sim::Model& model, const Area& target_area ) :
		Sensor( pn, par, model, target_area ),
		m_Body( *FindByName( model.GetBodies(), pn.GetStr( "body" ) ) )
		{
		}

		scone::Real SagittalPostureSensor::GetValue() const 
		{
			return m_Body.GetOri().ToExponentialMap().z;
		}

		scone::String SagittalPostureSensor::GetName() const 
		{
			return "Posture.S";
		}

	}
}

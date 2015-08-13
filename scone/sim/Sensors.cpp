#include "stdafx.h"
#include "Sensors.h"
#include "Model.h"
#include "Muscle.h"
#include "Area.h"
#include "Body.h"
#include "../cs/Tools.h"
#include "Dof.h"

namespace scone
{
	namespace sim
	{
		MuscleSensor::MuscleSensor( const PropNode& pn, opt::ParamSet& par, sim::Model& model, const Area& target_area ) :
		Sensor( pn, par, model, target_area ),
		m_Muscle( *FindByName( model.GetMuscles(), pn.GetStr( "muscle" ) ) )
		{
		}

		const String& MuscleSensor::GetSourceName() const 
		{
			return m_Muscle.GetName();
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

		scone::Real MuscleSpindleSensor::GetValue() const 
		{
			// derived from [Prochazka1999], but normalized to unit length
			return 0.105 * sqrt( std::max( 0.0, m_Muscle.GetNormalizedFiberVelocity() ) ) + m_Muscle.GetNormalizedFiberLength();
		}

		scone::String MuscleSpindleSensor::GetName() const 
		{
			return m_Muscle.GetName() + ".MS";
		}

		DofSensor::DofSensor( const PropNode& pn, opt::ParamSet& par, sim::Model& model, const Area& target_area ) :
		Sensor( pn, par, model, target_area ),
		m_Dof( *FindByName( model.GetDofs(), pn.GetStr( "dof" ) ) ),
		m_pRootDof( pn.HasKey( "root_dof" ) ? FindByName( model.GetDofs(), pn.GetStr( "root_dof" ) ).get() : nullptr )
		{
		}

		const String& DofSensor::GetSourceName() const 
		{
			return m_Dof.GetName();
		}

		scone::Real DofPositionSensor::GetValue() const 
		{
			// TODO: get rid of this if statement and use a "constant" Dof?
			if ( m_pRootDof )
				return m_pRootDof->GetPos() + m_Dof.GetPos();
			else return m_Dof.GetPos();
		}

		scone::String DofPositionSensor::GetName() const 
		{
			return m_Dof.GetName() + ".P";
		}

		scone::Real DofVelocitySensor::GetValue() const 
		{
			// TODO: get rid of this if statement and use a "constant" Dof?
			if ( m_pRootDof )
				return m_pRootDof->GetVel() + m_Dof.GetVel();
			else return m_Dof.GetVel();
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

		const String& LegLoadSensor::GetSourceName() const 
		{
			return m_Leg.GetName();
		}

		SagittalPostureSensor::SagittalPostureSensor( const PropNode& pn, opt::ParamSet& par, sim::Model& model, const Area& target_area ) :
		Sensor( pn, par, model, target_area ),
		m_PelvisTilt( *FindByName( model.GetDofs(), "pelvis_tilt" ) ),
		m_LumbarExtension( *FindByName( model.GetDofs(), "lumbar_extension" ) )
		{
			// TODO: get rid of hard-coded dof names
			// TODO: use body world position instead
		}

		scone::Real SagittalPostureSensor::GetValue() const 
		{
			return m_PelvisTilt.GetPos() + m_LumbarExtension.GetPos();
		}

		scone::String SagittalPostureSensor::GetName() const 
		{
			return "Posture.S";
		}

		const String& SagittalPostureSensor::GetSourceName() const 
		{
			return m_PelvisTilt.GetName();
		}
}
}

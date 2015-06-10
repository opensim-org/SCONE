#include "stdafx.h"
#include "BalanceSensor.h"
#include "Model.h"

namespace scone
{
	namespace sim
	{
		const StringMap< Index > BalanceSensor::m_SensorNames(
			BalanceSensor::SagittalBodyPostureSensor, "Posture" );

		BalanceSensor::BalanceSensor( Model& model ) :
		m_BodyDof( *FindByName( model.GetDofs(), "lumbar_extension" ) ),
		m_RootDof( *FindByName( model.GetDofs(), "pelvis_tilt" ) )
		{
			// both dofs are so hard-coded it's not funny...
			// TODO: less hard-coding
		}

		BalanceSensor::~BalanceSensor()
		{
		}

		scone::Real BalanceSensor::GetSensorValue( Index idx ) const
		{
			return m_RootDof.GetPos() + m_BodyDof.GetPos();
		}

		const StringIndexMap& BalanceSensor::GetSensorNames() const 
		{
			return m_SensorNames;
		}

		const String& BalanceSensor::GetName() const
		{
			static String str( "BalanceSensor" );
			return str;
		}

	}
}

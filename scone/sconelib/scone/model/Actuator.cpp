#include "Actuator.h"

namespace scone
{
	Actuator::Actuator() : m_ActuatorControlValue( 0.0 )
	{
	}

	Actuator::~Actuator()
	{
	}

	void Actuator::StoreData( Storage< Real >::Frame& frame )
	{
		frame[ GetName() + ".input" ] = GetInput();
	}
}

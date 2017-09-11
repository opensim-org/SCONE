#include "MotorNeuron.h"
#include "scone/model/Actuator.h"

namespace scone
{
	void MotorNeuron::UpdateActuator()
	{
		actuator_->AddInput( GetOutput() );
	}
}

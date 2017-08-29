#include "MotorNeuron.h"
#include "scone/model/Actuator.h"

namespace scone
{
	void MotorNeuron::UpdateActuator()
	{
		output_->AddInput( input_->GetOutput() );
	}
}

#include "MotorNeuron.h"

#include "scone/model/Actuator.h"
#include "scone/model/Model.h"
#include "scone/model/Muscle.h"
#include "../core/HasName.h"
#include "NeuralController.h"

namespace scone
{
	MotorNeuron::MotorNeuron( const PropNode& pn, Params& par, NeuralController& nc, const string& name ) :
	InterNeuron( pn, par, nc, name ),
	actuator_( FindByName( nc.GetModel().GetMuscles(), name ).get() )
	{
	}

	void MotorNeuron::UpdateActuator()
	{
		actuator_->AddInput( GetOutput() );
	}
}

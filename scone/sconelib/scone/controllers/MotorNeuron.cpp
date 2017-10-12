#include "MotorNeuron.h"

#include "scone/model/Actuator.h"
#include "scone/model/Model.h"
#include "scone/model/Muscle.h"
#include "../core/HasName.h"
#include "NeuralController.h"

namespace scone
{
	MotorNeuron::MotorNeuron( const PropNode& pn, Params& par, NeuralController& nc, const string& muscle, Index idx, Side side, const string& act_func ) :
	InterNeuron( pn, par, GetNameNoSide( muscle ), idx, side, act_func ),
	actuator_( FindByName( nc.GetModel().GetMuscles(), muscle ).get() )
	{
		name_ = muscle;
	}

	void MotorNeuron::UpdateActuator()
	{
		actuator_->AddInput( GetOutput() );
	}
}

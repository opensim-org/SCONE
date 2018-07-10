#include "MotorNeuron.h"

#include "scone/model/Actuator.h"
#include "scone/model/Model.h"
#include "scone/model/Muscle.h"
#include "../core/HasName.h"
#include "NeuralController.h"

namespace scone
{
	MotorNeuron::MotorNeuron( const PropNode& pn, Params& par, NeuralController& nc, const string& muscle, index_t idx, Side side, const string& act_func ) :
	Neuron( pn, idx, side, act_func )
	{
		name_ = muscle;
		muscle_ = FindByName( nc.GetModel().GetMuscles(), muscle ).get();

		ScopedParamSetPrefixer ps( par, GetNameNoSide( muscle ) + "." );
		offset_ = par.try_get( "C0", pn, "offset", 0.0 );
	}

	void MotorNeuron::UpdateActuator()
	{
		auto v = GetOutput();
		muscle_->AddInput( v );
	}
}

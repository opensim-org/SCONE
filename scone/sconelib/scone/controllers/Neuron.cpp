#include "Neuron.h"

#include "scone/model/Model.h"
#include "scone/model/Actuator.h"
#include "scone/model/SensorDelayAdapter.h"

namespace scone
{
	Neuron::Neuron( const PropNode& pn, Params& par, Model& model )
	{

	}

	void Neuron::UpdateOutput()
	{
		activation_t value = offset;
		for ( auto& i : inputs_ )
			value += i.first * *i.second;
		output_ = std::max( 0.0, value );
	}

	SensorNeuron::SensorNeuron( const PropNode& pn, Params& par, Model& model ) : output_()
	{
		INIT_PROP( pn, delay_, 999 );
	}

	void SensorNeuron::UpdateOutput()
	{
		output_ = intput_->GetValue( delay_ );
	}

	MotorNeuron::MotorNeuron( const PropNode& pn, Params& par, Model& model )
	{

	}

	void MotorNeuron::UpdateActuator()
	{
		output_->AddInput( *input_ );
	}
}

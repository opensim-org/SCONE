#include "Neuron.h"

#include <utility>

#include "scone/model/Model.h"
#include "scone/model/Actuator.h"
#include "scone/model/SensorDelayAdapter.h"
#include "NeuralController.h"
#include "spot/par_tools.h"
#include "scone/model/Sensors.h"
#include "scone/model/Muscle.h"
#include "scone/model/Dof.h"
#include "../optimization/Params.h"

namespace scone
{
	Neuron::Neuron( const PropNode& pn, Params& par, Model& model, NeuralController& controller, const Locality& loc ) :
	actuator_()
	{
		name_ = loc.ConvertName( pn.get< string >( "name", "NoName" ) );
		ScopedParamSetPrefixer sp( par, GetNameNoSide( name_ ) );

		INIT_PAR( pn, par, offset_, 0 );

		for ( auto& input_pn : pn )
		{
			if ( input_pn.first == "Input" )
			{
				activation_t* input = controller.AcquireInput( input_pn.second, par, model, loc );
				double gain = par.get( "gain", input_pn.second[ "gain" ] );
				inputs_.push_back( std::make_pair( gain, input ) );
			}
		}

		// set target actuator (if any)
		auto target = pn.get< string >( "target", "" );
		actuator_ = !target.empty() ? FindByName( model.GetActuators(), loc.ConvertName( target ) ) : nullptr;
	}

	void Neuron::UpdateOutput()
	{
		activation_t value = offset_;
		for ( auto& i : inputs_ )
			value += i.first * *i.second;
		output_ = std::max( 0.0, value );
		if ( actuator_ )
			actuator_->AddInput( output_ );
	}

	SensorNeuron::SensorNeuron( const PropNode& pn, Params& par, Model& model, Locality loc ) :
	output_(),
	input_(),
	offset_()
	{
		INIT_PROP( pn, delay_, 999 );
		const auto type = pn.get< string >( "type" );
		const auto source_name = pn.get< string >( "source" );
		const auto sided_source_name = loc.ConvertName( pn.get< string >( "source" ) );

		if ( type == "MuscleForce" )
			input_ = &model.AcquireDelayedSensor< MuscleForceSensor >( *FindByName( model.GetMuscles(), sided_source_name ) );
		else if ( type == "MuscleLength" )
		{
			input_ = &model.AcquireDelayedSensor< MuscleLengthSensor >( *FindByName( model.GetMuscles(), sided_source_name ) );
			offset_ = -1;
		}
		else if ( type == "DofPos" )
			input_ = &model.AcquireDelayedSensor< DofPositionSensor >( *FindByName( model.GetDofs(), source_name ) );
		else if ( type == "DofVel" )
			input_ = &model.AcquireDelayedSensor< DofVelocitySensor >( *FindByName( model.GetDofs(), source_name ) );
		else if ( type == "LegLoad" )
			input_ = &model.AcquireDelayedSensor< LegLoadSensor >( model.GetLeg( loc ) );

		flut_assert( input_ );
		name_ = input_->GetName();
	}

	void SensorNeuron::UpdateOutput()
	{
		output_ = input_->GetValue( delay_ ) + offset_;
	}
}

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
#include "scone/optimization/Params.h"
#include "flut/string_tools.hpp"
#include "scone/core/string_tools.h"

namespace scone
{
	InterNeuron::InterNeuron( const PropNode& pn, Params& par, Model& model, NeuralController& controller, const Locality& loc ) :
	Neuron( loc.ConvertName( pn.get< string >( "name" ) ) )
	{
		par_name_ = GetNameNoSide( name_ ) + '.';
		ScopedParamSetPrefixer sp( par, par_name_ );
		INIT_PAR( pn, par, offset_, 0 );

		for ( auto& input_pn : pn )
		{
			if ( input_pn.first == "Input" )
			{
				Neuron* input = controller.FindInput( input_pn.second, loc );
				if ( !input )
					input = controller.AddSensorNeuron( input_pn.second, par, model, loc );

				double gain = par.get( input->par_name_, input_pn.second[ "gain" ] );
				inputs_.push_back( std::make_pair( gain, input ) );
				log::info( name_, " <-- ", gain, " * ", input->name_ );
			}
		}

		// set target actuator (if any)
		if ( pn.has_key( "target" ) )
			controller.AddMotorNeuron( this, FindByName( model.GetActuators(), loc.ConvertName( pn.get< string >( "target" ) ) ) );
	}

	double InterNeuron::GetOutput() const
	{
		activation_t value = offset_;
		for ( auto& i : inputs_ )
			value += i.first * i.second->GetOutput();
		return output_ = std::max( 0.0, value );
	}

	SensorNeuron::SensorNeuron( const PropNode& pn, Params& par, Model& model, Locality loc ) :
	Neuron( "" ),
	input_()
	{
		INIT_PROP( pn, delay_, 999 );
		const auto type = pn.get< string >( "type" );
		const auto source_name = pn.get< string >( "source", "leg" );
		bool opposite = pn.get_any< bool >( { "mirrored", "opposite" }, false );
		par_name_ = source_name + ( opposite ? "_o." : "." ) + type;

		if ( opposite )
			loc = MakeMirrored( loc );

		if ( type == "F" )
		{
			const auto sided_source_name = loc.ConvertName( source_name );
			input_ = &model.AcquireDelayedSensor< MuscleForceSensor >( *FindByName( model.GetMuscles(), sided_source_name ) );
		}
		else if ( type == "L" )
		{
			const auto sided_source_name = loc.ConvertName( source_name );
			input_ = &model.AcquireDelayedSensor< MuscleLengthSensor >( *FindByName( model.GetMuscles(), sided_source_name ) );
			offset_ = -1;
		}
		else if ( type == "DP" )
		{
			input_ = &model.AcquireDelayedSensor< DofPositionSensor >( *FindByName( model.GetDofs(), source_name ) );
		}
		else if ( type == "DV" )
		{
			input_ = &model.AcquireDelayedSensor< DofVelocitySensor >( *FindByName( model.GetDofs(), source_name ) );
		}
		else if ( type == "LD" )
		{
			input_ = &model.AcquireDelayedSensor< LegLoadSensor >( model.GetLeg( loc ) );
		}

		flut_assert_msg( input_, "Unknown type " + type );
		name_ = input_->GetName() + stringf( "+%.0f", delay_ * 1000 );
	}

	double SensorNeuron::GetOutput() const
	{
		return output_ = input_->GetValue( delay_ ) + offset_;
	}

	void MotorNeuron::UpdateActuator()
	{
		output_->AddInput( input_->GetOutput() );
	}
}

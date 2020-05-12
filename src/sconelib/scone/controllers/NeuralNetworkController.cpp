#include "NeuralNetworkController.h"

#include "xo/string/string_tools.h"
#include "scone/model/Model.h"
#include "scone/model/SensorDelayAdapter.h"
#include "scone/model/Actuator.h"
#include "scone/model/Sensors.h"
#include "xo/utility/irange.h"
#include "scone/core/HasName.h"
#include "scone/model/MuscleId.h"

namespace scone::NN
{
	NeuralNetworkController::NeuralNetworkController( const PropNode& pn, Params& par, Model& model, const Location& area ) :
		Controller( pn, par, model, area ),
		INIT_MEMBER_REQUIRED( pn, neural_delays_ )
	{
		// create sensor neurons
		neurons_.emplace_back();
		for ( const auto& mus : model.GetMuscles() )
		{
			auto name = GetNameNoSide( mus->GetName() );
			AddSensor( &model.AcquireDelayedSensor<MuscleLengthSensor>( *mus ), neural_delays_[ name ], -1 );
			AddSensor( &model.AcquireDelayedSensor<MuscleForceSensor>( *mus ), neural_delays_[ name ], 0 );
		}
		auto name = "pelvis_tilt";
		auto kv = par.get( "pelvis_tilt.DPV.DV", 0.1, 0.01, -10, 10 );
		AddSensor( &model.AcquireDelayedSensor<DofPosVelSensor>( *FindByName( model.GetDofs(), name ), kv ), neural_delays_[ name ], 0 );

		// create inter neurons

		// create motor neurons
		neurons_.emplace_back();
		for ( const auto& mus : model.GetMuscles() )
		{
			auto& neuron = AddActuator( mus.get() );
			const auto par_name = GetNameNoSide( mus->GetName() );
			neuron.offset_ = par.get( par_name + ".C0", 0.0, 0.01, -10.0, 10.0 );
		}

		// create direct links
		links_.emplace_back();
		for ( index_t sidx : xo::irange( sensor_links_.size() ) )
		{
			for ( auto aidx : xo::irange( motor_links_.size() ) )
			{
				const auto& sname = xo::split_str_at_last( sensor_links_[ sidx ].sensor_->GetName(), "." );
				const auto& aname = motor_links_[ aidx ].actuator_->GetName();
				auto smi = MuscleId( sname.first );
				auto ami = MuscleId( aname );
				if ( smi.side_ == NoSide || smi.side_ == ami.side_ )
				{
					// do some name stuff
					auto parname = ami.base_;
					auto sparname = smi.base_;
					if ( parname != sparname )
						parname += '.' + sparname;
					parname += '.' + sname.second;

					// create the link
					double weight = par.get( parname, 0.0, 0.01, -10.0, 10.0 );
					links_.back().push_back( Link{ sidx, aidx, weight } );
				}
			}
		}
	}

	Neuron& NeuralNetworkController::AddSensor( SensorDelayAdapter* sensor, TimeInSeconds delay, double offset )
	{
		sensor_links_.push_back( SensorNeuronLink{ sensor, delay, offset, 1, neurons_.front().size() } );
		return neurons_.front().emplace_back( Neuron{ 0, offset, 0 } );
	}

	Neuron& NeuralNetworkController::AddActuator( Actuator* actuator )
	{
		motor_links_.push_back( MotorNeuronLink{ actuator, neurons_.back().size() } );
		return neurons_.back().emplace_back();
	}

	bool NeuralNetworkController::ComputeControls( Model& model, double timestamp )
	{
		// clear neuron inputs
		for ( auto& layer : neurons_ )
			for ( auto& n : layer )
				n.input_ = 0.0;

		// update input neurons with sensor values
		for ( const auto& sn : sensor_links_ )
			neurons_.front()[ sn.neuron_idx_ ].output_ = sn.sensor_->GetValue( sn.delay_ ) + neurons_.front()[ sn.neuron_idx_ ].offset_;

		// update links and inter neurons
		for ( index_t idx = 0; idx < links_.size(); ++idx )
		{
			for ( const auto& link : links_[ idx ] )
				neurons_[ idx + 1 ][ link.trg_idx_ ].input_ += link.weight_ * neurons_[ idx ][ link.src_idx_ ].output_;
			for ( auto& neuron : neurons_[ idx + 1 ] )
				update_output( neuron );
		}

		// update actuators with output neurons
		for ( auto& mn : motor_links_ )
			mn.actuator_->AddInput( neurons_.back()[ mn.neuron_idx_ ].output_ );

		return false;
	}

	void NeuralNetworkController::StoreData( Storage<Real>::Frame& frame, const StoreDataFlags& flags ) const
	{
		for ( const auto& link : links_.back() )
		{
			auto& sl = sensor_links_[ link.src_idx_ ];
			auto& al = motor_links_[ link.trg_idx_ ];
			String str = "MN." + al.actuator_->GetName() + '.' + sl.sensor_->GetName();
			frame[ str ] = link.weight_ * neurons_.front()[ link.src_idx_ ].output_;
		}
	}

	String NeuralNetworkController::GetClassSignature() const
	{
		return xo::stringf( "NN%d", neurons_.size() );
	}
}

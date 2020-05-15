#include "NeuralNetworkController.h"

#include "xo/string/string_tools.h"
#include "scone/core/Log.h"
#include "scone/model/Model.h"
#include "scone/model/Muscle.h"
#include "scone/model/Dof.h"
#include "scone/model/SensorDelayAdapter.h"
#include "scone/model/Actuator.h"
#include "scone/model/Sensors.h"
#include "xo/utility/irange.h"
#include "scone/core/HasName.h"
#include "scone/model/MuscleId.h"
#include "xo/utility/hash.h"
#include <numeric>

namespace scone::NN
{
	NeuralNetworkController::NeuralNetworkController( const PropNode& pn, Params& par, Model& model, const Location& area ) :
		Controller( pn, par, model, area ),
		INIT_MEMBER_REQUIRED( pn, neural_delays_ )
	{
		for ( const auto& [key, child_pn] : pn )
		{
			try {
				CreateComponent( key, child_pn, par, model );
			}
			catch ( std::exception& e ) {
				SCONE_ERROR( "Error in " + key + ": " + e.what() );
			}
		}
	}

	NeuronLayer& NeuralNetworkController::AddNeuronLayer( index_t layer )
	{
		neurons_.resize( std::max( neurons_.size(), layer + 1 ) );
		return neurons_[ layer ];
	}

	LinkLayer& NeuralNetworkController::AddLinkLayer( index_t input_layer, index_t output_layer )
	{
		SCONE_ASSERT( output_layer > 0 );
		links_.resize( std::max( links_.size(), output_layer ) );
		return links_[ output_layer - 1 ].emplace_back( input_layer );
	}

	Neuron& NeuralNetworkController::AddSensor( SensorDelayAdapter* sensor, TimeInSeconds delay, double offset )
	{
		MuscleSensor* ms = dynamic_cast<MuscleSensor*>( &sensor->GetInputSensor() );
		sensor_links_.push_back( SensorNeuronLink{ sensor, delay, offset, 1, neurons_.front().size(), ms ? &ms->muscle_ : nullptr } );
		return neurons_.front().emplace_back( offset );
	}

	Neuron& NeuralNetworkController::AddActuator( Actuator* actuator, double offset )
	{
		motor_links_.push_back( MotorNeuronLink{ actuator, neurons_.back().size(), dynamic_cast<Muscle*>( actuator ) } );
		return neurons_.back().emplace_back( offset );
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
			for ( const auto& link_layer : links_[ idx ] )
				for ( const auto& link : link_layer.links_ )
					neurons_[ idx + 1 ][ link.trg_idx_ ].input_ += link.weight_ * neurons_[ link_layer.input_layer_ ][ link.src_idx_ ].output_;
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
		for ( const auto& sn : sensor_links_ )
			frame[ "SN." + sn.sensor_->GetName() ] = neurons_.front()[ sn.neuron_idx_ ].output_;

		for ( auto& mn : motor_links_ )
			frame[ "MN." + mn.actuator_->GetName() ] = neurons_.back()[ mn.neuron_idx_ ].output_;
	}

	PropNode NeuralNetworkController::GetInfo() const
	{
		PropNode pn;
		for ( const auto& sn : sensor_links_ )
			pn[ sn.sensor_->GetName() ] = neurons_.front()[ sn.neuron_idx_ ].offset_;

		for ( const auto& il : links_.front().front().links_ )
		{
			PropNode lpn;
			lpn[ "src_idx" ] = il.src_idx_;
			lpn[ "trg_idx" ] = il.trg_idx_;
			lpn[ "weight" ] = il.weight_;
			pn.add_child( "link", lpn );
		}

		for ( const auto& mn : motor_links_ )
			pn[ mn.actuator_->GetName() ] = neurons_.back()[ mn.neuron_idx_ ].offset_;

		return pn;
	}

	String NeuralNetworkController::GetClassSignature() const
	{
		size_t links = 0;
		for ( const auto& llv : links_ )
			for ( const auto& ll : llv )
				links += ll.links_.size();
		return xo::stringf( "NN%d", links );
	}

	String NeuralNetworkController::GetParName( const String& source, const String& target, const String& type, bool use_muscle_lines )
	{
		auto sid = MuscleId( source );
		auto tid = MuscleId( target );
		auto sns = use_muscle_lines ? sid.base_line_name() : sid.base_;
		auto tns = use_muscle_lines ? tid.base_line_name() : tid.base_;
		String postfix = type.empty() ? "" : '.' + type;
		if ( tns == sns )
			return tns + postfix;
		else return tns + '.' + sns + postfix;
	}

	String NeuralNetworkController::GetNeuronName( index_t layer_idx, index_t neuron_idx )
	{
		if ( !sensor_links_.empty() && layer_idx == 0 )
			return sensor_links_[ neuron_idx ].sensor_->GetName();
		else if ( !motor_links_.empty() && layer_idx == neurons_.size() - 1 )
			return motor_links_[ neuron_idx ].actuator_->GetName();
		else
		{
			auto lr_threshold = neurons_[ layer_idx ].size() / 2;
			return xo::stringf( "I%d_%d_%c", layer_idx, neuron_idx % lr_threshold, neuron_idx < lr_threshold ? 'l' : 'r' );
		}
	}

	void NeuralNetworkController::CreateComponent( const String& key, const PropNode& pn, Params& par, Model& model )
	{
		switch ( xo::hash( key ) )
		{
		case "MuscleSensors"_hash:
		{
			AddNeuronLayer( 0 );
			bool length = pn.get<bool>( "length", true );
			bool force = pn.get<bool>( "force", true );
			auto include = pn.get<xo::pattern_matcher>( "include", "" );
			for ( const auto& mus : model.GetMuscles() )
			{
				if ( include.empty() || include( mus->GetName() ) )
				{
					auto delay = neural_delays_[ GetNameNoSide( mus->GetName() ) ];
					if ( length ) AddSensor( &model.AcquireDelayedSensor<MuscleLengthSensor>( *mus ), delay, -1 );
					if ( force ) AddSensor( &model.AcquireDelayedSensor<MuscleForceSensor>( *mus ), delay, 0 );
				}
			}
			break;
		}
		case "BodyAngularVelocitySensor"_hash:
		{
			AddSensor( &model.AcquireDelayedSensor<BodyAngularVelocitySensor>(
				*FindByName( model.GetBodies(), pn.get<String>( "body" ) ),
				pn.get<Vec3>( "dir" ), pn.get<String>( "id" ) ), pn.get<double>( "delay" ), 0 );
			break;
		}
		case "BodyOrientationSensor"_hash:
		{
			AddSensor( &model.AcquireDelayedSensor<BodyOrientationSensor>(
				*FindByName( model.GetBodies(), pn.get<String>( "body" ) ),
				pn.get<Vec3>( "dir" ), pn.get<String>( "id" ) ), pn.get<double>( "delay" ), 0 );
			break;
		}
		case "DofPosVelSensor"_hash:
		{
			auto dof_name = pn.get<String>( "dof" );
			auto kv = par.try_get( dof_name + ".DPV.DV", pn, "velocity_gain", 0.1 );
			const auto& dof = *FindByName( model.GetDofs(), dof_name );
			Dof* parent_dof = pn.has_key( "parent_dof" ) ? &*FindByName( model.GetDofs(), pn.get<String>( "parent_dof" ) ) : nullptr;
			if ( pn.get<bool>( "dual_sided", false ) )
			{
				AddSensor( &model.AcquireDelayedSensor<DofPosVelSensor>( dof, kv, parent_dof, LeftSide ), neural_delays_[ dof_name ], 0 );
				AddSensor( &model.AcquireDelayedSensor<DofPosVelSensor>( dof, kv, parent_dof, RightSide ), neural_delays_[ dof_name ], 0 );
			}
			else AddSensor( &model.AcquireDelayedSensor<DofPosVelSensor>( dof, kv, parent_dof ), neural_delays_[ dof_name ], 0 );
			break;
		}
		case "InterNeurons"_hash:
		{
			const auto layer_idx = pn.get<index_t>( "layer", neurons_.size() );
			auto& layer = AddNeuronLayer( layer_idx );
			const auto neurons = pn.get<index_t>( "neurons" ) * 2; // both left and right sides
			const auto& offset = pn.get_child( "offset" );
			layer.resize( neurons ); // first resize so GetNeuronName knows who's left and right
			for ( index_t idx = 0; idx < neurons; ++idx )
				layer[ idx ].offset_ = par.get( GetNameNoSide( GetNeuronName( layer_idx, idx ) ) + ".C0", offset );
			break;
		}
		case "MotorNeurons"_hash:
		{
			AddNeuronLayer( pn.get<index_t>( "layer" ) );
			auto include = pn.get<xo::pattern_matcher>( "include", "" );
			for ( const auto& mus : model.GetMuscles() )
			{
				if ( include.empty() || include( mus->GetName() ) )
				{
					auto parname = GetNameNoSide( mus->GetName() ) + ".C0";
					AddActuator( mus.get(), par.get( parname, pn.get_child( "offset" ) ) );
				}
			}
			break;
		}
		case "Link"_hash:
		{
			auto input_layer_idx = pn.get<index_t>( "input_layer", 0 );
			auto output_layer_idx = pn.get<index_t>( "output_layer", neurons_.size() - 1 );
			auto& link_layer = AddLinkLayer( input_layer_idx, output_layer_idx );
			bool sensor_motor_link = input_layer_idx == 0 && output_layer_idx == neurons_.size() - 1;
			bool use_muscle_lines = pn.get<bool>( "use_muscle_lines", true );
			auto input_include = pn.try_get<xo::pattern_matcher>( "input_include" );
			auto output_include = pn.try_get<xo::pattern_matcher>( "output_include" );
			for ( auto target_neuron_idx : xo::irange( neurons_[ output_layer_idx ].size() ) )
			{
				const auto target_name = GetNeuronName( output_layer_idx, target_neuron_idx );
				if ( output_include && !output_include->match( target_name ) )
					continue; // skip this neuron

				for ( auto source_neuron_idx : xo::irange( neurons_[ input_layer_idx ].size() ) )
				{
					const auto source_name_full = GetNeuronName( input_layer_idx, source_neuron_idx );
					if ( input_include && !input_include->match( source_name_full ) )
						continue; // skip this neuron

					auto [source_name, source_type] = xo::split_str_at_last( source_name_full, "." );
					auto src_side = GetSideFromName( source_name );
					auto trg_side = GetSideFromName( target_name );

					if ( pn.get<bool>( "same_side", true ) )
						if ( !src_side == NoSide && src_side != trg_side )
							continue; // neuron not on same side

					if ( sensor_motor_link )
					{
						const auto& sl = sensor_links_[ source_neuron_idx ];
						const auto& ml = motor_links_[ target_neuron_idx ];
						if ( pn.get<bool>( "shared_joint", false ) )
							if ( sl.muscle_ && ml.muscle_ && !sl.muscle_->HasSharedJoints( *ml.muscle_ ) )
								continue;
					}

					// if we arrive here there's actually a connection
					auto parname = GetParName( source_name, target_name, source_type, use_muscle_lines );
					double weight = par.get( parname, pn.get_child( "weight" ) );
					link_layer.links_.push_back( Link{ source_neuron_idx, target_neuron_idx, weight } );
				}
			}
		}
		default:
			break;
		}
	}
}

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
#include "scone/core/profiler_config.h"
#include <algorithm>

namespace scone::NN
{
	NeuralNetworkController::NeuralNetworkController( const PropNode& pn, Params& par, Model& model, const Location& area ) :
		Controller( pn, par, model, area ),
		INIT_MEMBER_REQUIRED( pn, neural_delays_ ),
		INIT_MEMBER( pn, parameter_aliases_, {} ),
		INIT_PAR_MEMBER( pn, par, leakyness_, 0.01 ),
		INIT_MEMBER( pn, ignore_muscle_lines_, false ),
		INIT_MEMBER( pn, symmetric_, true )
	{
		SCONE_PROFILE_FUNCTION( model.GetProfiler() );

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
		neuron_names_.resize( neurons_.size() );
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
		sensor_links_.push_back( SensorNeuronLink{ sensor, delay, neurons_.front().size(), ms ? &ms->muscle_ : nullptr } );
		return neurons_.front().emplace_back( offset );
	}

	Neuron& NeuralNetworkController::AddActuator( Actuator* actuator, double offset )
	{
		motor_links_.push_back( MotorNeuronLink{ actuator, neurons_.back().size(), dynamic_cast<Muscle*>( actuator ) } );
		return neurons_.back().emplace_back( offset );
	}

	bool NeuralNetworkController::ComputeControls( Model& model, double timestamp )
	{
		SCONE_PROFILE_FUNCTION( model.GetProfiler() );

		// clear neuron inputs
		for ( auto& layer : neurons_ )
			for ( auto& n : layer )
				n.input_ = 0.0;

		// update sensor neurons with sensor values
		for ( const auto& sn : sensor_links_ )
			neurons_.front()[ sn.neuron_idx_ ].output_ = sn.sensor_->GetValue( sn.delay_ ) + neurons_.front()[ sn.neuron_idx_ ].offset_;

		// update links and inter neurons
		for ( index_t idx = 0; idx < links_.size(); ++idx )
		{
			auto& output_layer = neurons_[ idx + 1 ];
			for ( const auto& link_layer : links_[ idx ] )
			{
				auto& input_layer = neurons_[ link_layer.input_layer_ ];
				for ( const auto& link : link_layer.links_ )
					output_layer[ link.trg_idx_ ].input_ += link.weight_ * input_layer[ link.src_idx_ ].output_;
			}

			output_layer.update_outputs();
		}

		// update actuators with output neurons
		for ( auto& mn : motor_links_ )
			mn.actuator_->AddInput( neurons_.back()[ mn.neuron_idx_ ].output_ );

		return false;
	}

	void NeuralNetworkController::StoreData( Storage<Real>::Frame& frame, const StoreDataFlags& flags ) const
	{
		for ( auto lidx : xo::size_range( neurons_ ) )
			for ( auto nidx : xo::size_range( neurons_[ lidx ] ) )
				frame[ GetNeuronName( lidx, nidx ) ] = neurons_[ lidx ][ nidx ].output_;
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

	const String& NeuralNetworkController::GetParAlias( const String& name )
	{
		auto it = xo::find_if( parameter_aliases_, [&]( const auto& e ) { return xo::str_begins_with( name, e.first ); } );
		if ( it != parameter_aliases_.end() )
			return it->second;
		else return name;
	}

	String NeuralNetworkController::GetParName( const String& name, bool ignore_muscle_lines, bool symmetric )
	{
		String par_name;
		auto mid = MuscleId( name );
		if ( ignore_muscle_lines )
			par_name = GetParAlias( mid.base_ );
		else par_name = mid.base_line_name();
		if ( !symmetric )
			par_name += mid.side_name();
		return par_name;
	}

	String NeuralNetworkController::GetParName( const String& source, const String& target, const String& type, bool ignore_muscle_lines, bool symmetric )
	{
		auto sname = GetParName( source, ignore_muscle_lines, symmetric );
		auto tname = GetParName( target, ignore_muscle_lines, symmetric );
		String postfix = type.empty() ? "" : '.' + type;
		if ( sname == tname )
			return tname + postfix;
		else return tname + '.' + sname + postfix;
	}

	String NeuralNetworkController::GetNeuronName( index_t layer_idx, index_t neuron_idx ) const
	{
		if ( !sensor_links_.empty() && layer_idx == 0 )
			return sensor_links_[ neuron_idx ].sensor_->GetName();
		else if ( !motor_links_.empty() && layer_idx == neurons_.size() - 1 )
			return motor_links_[ neuron_idx ].actuator_->GetName();
		else return neuron_names_[ layer_idx ][ neuron_idx ];
	}

	void NeuralNetworkController::CreateComponent( const String& key, const PropNode& pn, Params& par, Model& model )
	{
		switch ( xo::hash( key ) )
		{
		case "MuscleSensors"_hash:
		{
			AddNeuronLayer( 0 );
			bool force = pn.get<bool>( "force", false );
			bool length = pn.get<bool>( "length", false );
			bool velocity = pn.get<bool>( "velocity", false );
			bool length_velocity = pn.get<bool>( "length_velocity", false );
			bool length_velocity_sqrt = pn.get<bool>( "length_velocity_sqrt", false );
			auto include = pn.get<xo::pattern_matcher>( "include", "" );
			const bool ignore_muscle_lines = pn.get<bool>( "ignore_muscle_lines", true ); // defaults to true for back comp
			const bool symmetric = pn.get<bool>( "symmetric", symmetric_ );
			for ( const auto& mus : model.GetMuscles() )
			{
				if ( include.empty() || include( mus->GetName() ) )
				{
					auto musid = MuscleId( mus->GetName() );
					auto musparname = GetParName( mus->GetName(), ignore_muscle_lines, symmetric );
					auto delay = neural_delays_[ musid.base_line_name() ];
					auto lofs = -pn.get<double>( "L0", 1.0 ); // defaults to -1
					if ( force ) AddSensor( &model.AcquireDelayedSensor<MuscleForceSensor>( *mus ), delay, 0 );
					if ( length ) AddSensor( &model.AcquireDelayedSensor<MuscleLengthSensor>( *mus ), delay, lofs );
					if ( velocity ) AddSensor( &model.AcquireDelayedSensor<MuscleVelocitySensor>( *mus ), delay, 0 );
					if ( length_velocity || length_velocity_sqrt ) {
						auto kv = par.try_get( musparname + ".KV", pn, "velocity_gain", 0.1 );
						if ( length_velocity )
							AddSensor( &model.AcquireDelayedSensor<MuscleLengthVelocitySensor>( *mus, kv ), delay, lofs );
						else AddSensor( &model.AcquireDelayedSensor<MuscleLengthVelocitySqrtSensor>( *mus, kv ), delay, lofs );
					}
				}
			}
			break;
		}
		case "BodyOrientationSensor"_hash:
		{
			const auto& body = *FindByName( model.GetBodies(), pn.get<String>( "body" ) );
			for ( auto side : { LeftSide, RightSide } )
				AddSensor(
					&model.AcquireDelayedSensor<BodyOrientationSensor>( body, pn.get<Vec3>( "dir" ), pn.get<String>( "postfix" ), side ),
					pn.get<double>( "delay" ), 0 );
			break;
		}
		case "BodyAngularVelocitySensor"_hash:
		{
			const auto& body = *FindByName( model.GetBodies(), pn.get<String>( "body" ) );
			for ( auto side : { LeftSide, RightSide } )
				AddSensor(
					&model.AcquireDelayedSensor<BodyAngularVelocitySensor>( body, pn.get<Vec3>( "dir" ), pn.get<String>( "postfix" ), side ),
					pn.get<double>( "delay" ), 0 );
			break;
		}
		case "BodyOriVelSensor"_hash:
		{
			const auto body_name = pn.get<String>( "body" );
			const auto postfix = pn.get<String>( "postfix" );
			const auto& body = *FindByName( model.GetBodies(), body_name );
			auto kv = par.try_get( body_name + postfix + ".KV", pn, "velocity_gain", 0.1 );
			for ( auto side : { LeftSide, RightSide } )
				AddSensor(
					&model.AcquireDelayedSensor<BodyOriVelSensor>( body, pn.get<Vec3>( "dir" ), kv, postfix, side ),
					pn.get<double>( "delay" ), 0 );
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
			layer.update_func_ = make_update_function( pn.get<String>( "activation", "leaky_relu" ) );
			const bool symmetric = pn.get<bool>( "symmetric", symmetric_ );

			// set names of interneurons
			auto& neuron_names = neuron_names_[ layer_idx ];
			if ( auto neurons = pn.try_get<index_t>( "neurons" ) )
			{
				neuron_names.reserve( *neurons * 2 );
				for ( auto s : { LeftSide, RightSide } )
					for ( index_t idx = 0; idx < *neurons; ++idx )
						neuron_names.emplace_back( xo::stringf( "I%d_%d_%c", layer_idx, idx, s == LeftSide ? 'l' : 'r' ) );
			}
			else if ( const auto names = pn.try_get<String>( "names" ) )
			{
				auto base_names = xo::split_str( *names, " ;," );
				neuron_names.reserve( base_names.size() * 2 );
				for ( auto s : { LeftSide, RightSide } )
					for ( const auto& name : base_names )
						neuron_names.emplace_back( name + GetSideName( s ) );
			}

			// neuron count is based on names
			const auto neurons = neuron_names.size();
			const auto& offset = pn.get_child( "offset" );
			layer.resize( neurons );
			for ( index_t idx = 0; idx < neurons; ++idx )
			{
				// #perf: avoid the copy by having GetNeuronName return refs
				String neuronname = GetNeuronName( layer_idx, idx );
				String parname = ( symmetric ? GetNameNoSide( neuronname ) : neuronname ) + ".C0";
				layer[ idx ].offset_ = par.get( parname, offset );
			}
			break;
		}
		case "MotorNeurons"_hash:
		{
			auto& layer = AddNeuronLayer( pn.get<index_t>( "layer" ) );
			const bool ignore_muscle_lines = pn.get<bool>( "ignore_muscle_lines", false ); // defaults to false for back comp
			const bool symmetric = pn.get<bool>( "symmetric", symmetric_ );
			layer.update_func_ = make_update_function( pn.get<String>( "activation", "relu" ) );
			auto include = pn.get<xo::pattern_matcher>( "include", "" );
			for ( const auto& mus : model.GetMuscles() )
			{
				if ( include.empty() || include( mus->GetName() ) )
				{
					auto parname = GetParName( mus->GetName(), ignore_muscle_lines, symmetric ) + ".C0";
					AddActuator( mus.get(), par.get( parname, pn.get_child( "offset" ) ) );
				}
			}
			break;
		}
		case "Link"_hash:
			CreateLinkComponent( pn, par, model );
			break;
		default:
			break;
		}
	}

	void NeuralNetworkController::CreateLinkComponent( const PropNode& pn, Params& par, Model& model )
	{
		SCONE_PROFILE_FUNCTION( model.GetProfiler() );

		auto input_layer_idx = pn.get<index_t>( "input_layer", 0 );
		auto output_layer_idx = pn.get<index_t>( "output_layer", neurons_.size() - 1 );
		auto& link_layer = AddLinkLayer( input_layer_idx, output_layer_idx );
		bool sensor_motor_link = input_layer_idx == 0 && output_layer_idx == neurons_.size() - 1;
		const bool ignore_muscle_lines = pn.get<bool>( "ignore_muscle_lines", ignore_muscle_lines_ );
		const bool symmetric = pn.get<bool>( "symmetric", symmetric_ );
		auto input_include = pn.try_get_any<xo::pattern_matcher>( { "input_include", "input" } );
		auto output_include = pn.try_get_any<xo::pattern_matcher>( { "output_include", "output" } );
		auto input_type = pn.try_get<String>( "type" );
		auto same_side = pn.get<bool>( "same_side", true );
		auto same_name = pn.get<bool>( "same_name", false );

		for ( auto target_neuron_idx : xo::irange( neurons_[ output_layer_idx ].size() ) )
		{
			const auto target_name = GetNeuronName( output_layer_idx, target_neuron_idx );
			if ( output_include && !output_include->match( target_name ) )
				continue; // skip, not part of output pattern

			for ( auto source_neuron_idx : xo::irange( neurons_[ input_layer_idx ].size() ) )
			{
				const auto source_name_full = GetNeuronName( input_layer_idx, source_neuron_idx );
				if ( input_include && !input_include->match( source_name_full ) )
					continue; // skip, not part of input pattern

				auto [source_name, source_type] = xo::split_str_at_last( source_name_full, "." );
				if ( input_type && source_type != *input_type )
					continue; // skip, wrong type

				auto src_side = GetSideFromName( source_name );
				auto trg_side = GetSideFromName( target_name );

				if ( same_side && !src_side == NoSide && src_side != trg_side )
					continue; // neuron not on same side

				if ( same_name && GetNameNoSide( source_name ) != GetNameNoSide( target_name) )
					continue;

				if ( sensor_motor_link )
				{
					const auto& sl = sensor_links_[ source_neuron_idx ];
					const auto& ml = motor_links_[ target_neuron_idx ];
					if ( pn.get<bool>( "shared_joint", false ) )
						if ( sl.muscle_ && ml.muscle_ && !sl.muscle_->HasSharedJoints( *ml.muscle_ ) )
							continue;
				}

				// if we arrive here there's actually a connection
				auto parname = GetParName( source_name, target_name, source_type, ignore_muscle_lines, symmetric );
				double weight = par.get( parname, pn.get_child( "weight" ) );
				link_layer.links_.push_back( Link{ source_neuron_idx, target_neuron_idx, weight } );
			}
		}
	}
}

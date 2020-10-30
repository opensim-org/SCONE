#include "NeuralNetworkController.h"

#include "xo/string/string_tools.h"
#include "xo/utility/irange.h"
#include "xo/utility/hash.h"
#include "xo/utility/factory.h"

#include "scone/core/Log.h"
#include "scone/model/Model.h"
#include "scone/model/Muscle.h"
#include "scone/model/Dof.h"
#include "scone/model/SensorDelayAdapter.h"
#include "scone/model/Actuator.h"
#include "scone/model/Sensors.h"
#include "scone/core/HasName.h"
#include "scone/model/MuscleId.h"
#include "scone/core/profiler_config.h"

namespace scone::NN
{
	struct linear { static double update( const double v ) { return v; } };
	struct relu { static double update( const double v ) { return std::max( 0.0, v ); } };
	struct leaky_relu { static double update( const double v ) { return v >= 0.0 ? v : 0.01 * v; } };
	struct tanh { static double update( const double v ) { return std::tanh( v ); } };
	struct tanh_norm { static double update( const double v ) { return 0.5 * std::tanh( 2.0 * v - 1.0 ) + 0.5; } };
	struct tanh_norm_01 { static double update( const double v ) { return 0.495 * std::tanh( 2.0 * v - 1.0 ) + 0.505; } };

	using OutputUpdaterFactory = xo::factory<OutputUpdater, const PropNode&>;
	u_ptr<OutputUpdater> make_update_function( const PropNode& pn, const String& default_activation )
	{
		static OutputUpdaterFactory fac = OutputUpdaterFactory()
			.register_type<BasicOutputUpdater<linear>>( "linear" )
			.register_type<BasicOutputUpdater<relu>>( "relu" )
			.register_type<BasicOutputUpdater<leaky_relu>>( "leaky_relu" )
			.register_type<BasicOutputUpdater<tanh>>( "tanh" )
			.register_type<BasicOutputUpdater<tanh_norm>>( "tanh_norm" )
			.register_type<BasicOutputUpdater<tanh_norm_01>>( "tanh_norm_01" )
			.register_type<DynamicOutputUpdater<leaky_relu>>( "dyn_leaky_relu" );

		return fac.create( pn.get<String>( "activation", default_activation ), pn );
	}

	NeuralNetworkController::NeuralNetworkController( const PropNode& pn, Params& par, Model& model, const Location& area ) :
		Controller( pn, par, model, area ),
		INIT_MEMBER_REQUIRED( pn, neural_delays_ ),
		INIT_MEMBER( pn, parameter_aliases_, {} ),
		INIT_PAR_MEMBER( pn, par, leakyness_, 0.01 ),
		INIT_MEMBER( pn, ignore_muscle_lines_, false ),
		INIT_MEMBER( pn, symmetric_, true )
	{
		SCONE_PROFILE_FUNCTION( model.GetProfiler() );

		// create initial layer for sensors
		layers_.resize( 1 );
		layers_.front().layer_idx_ = 0;

		// create additional components
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

	NeuronLayer& NeuralNetworkController::AddNeuronLayer( const PropNode& pn, const String& default_activation )
	{
		const auto idx = pn.get<index_t>( "layer", layers_.size() );
		layers_.resize( std::max( layers_.size(), idx + 1 ) );
		auto& layer = layers_[ idx ];
		layer.layer_idx_ = idx;
		if ( !layer.update_func_ )
			layer.update_func_ = make_update_function( pn, default_activation );
		return layer;
	}

	LinkLayer& NeuralNetworkController::AddLinkLayer( index_t input_layer, index_t output_layer )
	{
		SCONE_ASSERT( output_layer > 0 );
		links_.resize( std::max( links_.size(), output_layer ) );
		return links_[ output_layer - 1 ].emplace_back( input_layer );
	}

	Neuron& NeuralNetworkController::AddSensor( SensorDelayAdapter* sensor, TimeInSeconds delay, double offset )
	{
		SCONE_ERROR_IF( layers_.empty(), "No SensorNeuron layer defined" );
		MuscleSensor* ms = dynamic_cast<MuscleSensor*>( &sensor->GetInputSensor() );
		auto& layer = layers_.front();
		sensor_links_.push_back( SensorNeuronLink{ sensor, delay, layer.neurons_.size(), ms ? &ms->muscle_ : nullptr } );
		layer.names_.emplace_back( sensor->GetName() );
		return layer.neurons_.emplace_back( offset );
	}

	Neuron& NeuralNetworkController::AddActuator( Actuator* actuator, double offset )
	{
		SCONE_ERROR_IF( motor_layer_ == no_index, "No MotorNeuron layer defined" );
		auto& layer = layers_[ motor_layer_ ];
		motor_links_.push_back( MotorNeuronLink{ actuator, layer.neurons_.size(), dynamic_cast<Muscle*>( actuator ) } );
		layer.names_.emplace_back( actuator->GetName() );
		return layer.neurons_.emplace_back( offset );
	}

	bool NeuralNetworkController::ComputeControls( Model& model, double timestamp )
	{
		SCONE_PROFILE_FUNCTION( model.GetProfiler() );

		// clear neuron inputs
		for ( auto& layer : layers_ )
			for ( auto& n : layer.neurons_ )
				n.input_ = 0.0;

		// update sensor neurons with sensor values
		auto& sensor_neurons = layers_.front().neurons_;
		for ( const auto& sl : sensor_links_ )
			sensor_neurons[ sl.neuron_idx_ ].output_ = sl.sensor_->GetValue( sl.delay_ ) + sensor_neurons[ sl.neuron_idx_ ].offset_;

		// update links and inter neurons
		for ( index_t idx = 0; idx < links_.size(); ++idx )
		{
			auto& target_layer = layers_[ idx + 1 ];
			for ( const auto& link_layer : links_[ idx ] )
			{
				auto& source_layer = layers_[ link_layer.input_layer_ ];
				for ( const auto& link : link_layer.links_ )
					target_layer.neurons_[ link.trg_idx_ ].input_ += link.weight_ * source_layer.neurons_[ link.src_idx_ ].output_;
			}

			// update outputs
			target_layer.update_func_->Update( target_layer.neurons_, model.GetDeltaTime() );
		}

		// update actuators with output neurons
		auto& motor_neurons = layers_[ motor_layer_ ].neurons_;
		for ( auto& ml : motor_links_ )
			ml.actuator_->AddInput( motor_neurons[ ml.neuron_idx_ ].output_ );

		return false;
	}

	void NeuralNetworkController::StoreData( Storage<Real>::Frame& frame, const StoreDataFlags& flags ) const
	{
		for ( auto lidx : xo::size_range( layers_ ) )
			for ( auto nidx : xo::size_range( layers_[ lidx ].neurons_ ) )
				frame[ GetNeuronName( lidx, nidx ) ] = layers_[ lidx ].neurons_[ nidx ].output_;
	}

	PropNode NeuralNetworkController::GetInfo() const
	{
		PropNode pn;
		for ( const auto& sn : sensor_links_ )
			pn[ sn.sensor_->GetName() ] = layers_.front().neurons_[ sn.neuron_idx_ ].offset_;

		for ( const auto& il : links_.front().front().links_ )
		{
			PropNode lpn;
			lpn[ "src_idx" ] = il.src_idx_;
			lpn[ "trg_idx" ] = il.trg_idx_;
			lpn[ "weight" ] = il.weight_;
			pn.add_child( "link", lpn );
		}

		for ( const auto& mn : motor_links_ )
			pn[ mn.actuator_->GetName() ] = layers_[ motor_layer_ ].neurons_[ mn.neuron_idx_ ].offset_;

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
		String postfix;
		if ( GetSideFromName( source ) != GetSideFromName( target ) )
			postfix = "_o";
		if ( !type.empty() )
			postfix += '.' + type;
		if ( sname == tname )
			return tname + postfix;
		else return tname + '.' + sname + postfix;
	}

	const String& NeuralNetworkController::GetNeuronName( index_t layer_idx, index_t neuron_idx ) const
	{
		SCONE_ASSERT( layer_idx < layers_.size() && neuron_idx < layers_[ layer_idx ].neurons_.size() );
		return layers_[ layer_idx ].names_[ neuron_idx ];
	}

	void NeuralNetworkController::CreateComponent( const String& key, const PropNode& pn, Params& par, Model& model )
	{
		switch ( xo::hash( key ) )
		{
		case "MuscleSensors"_hash:
		{
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
			for ( auto side : { RightSide, LeftSide } )
				AddSensor(
					&model.AcquireDelayedSensor<BodyOrientationSensor>( body, pn.get<Vec3>( "dir" ), pn.get<String>( "postfix" ), side ),
					pn.get<double>( "delay" ), 0 );
			break;
		}
		case "BodyAngularVelocitySensor"_hash:
		{
			const auto& body = *FindByName( model.GetBodies(), pn.get<String>( "body" ) );
			for ( auto side : { RightSide, LeftSide } )
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
			const auto target = par.try_get( body_name + postfix + ".P0", pn, "target", 0 );;
			for ( auto side : { RightSide, LeftSide } )
				AddSensor(
					&model.AcquireDelayedSensor<BodyOriVelSensor>( body, pn.get<Vec3>( "dir" ), kv, postfix, side, target ),
					pn.get<double>( "delay" ), 0 );
			break;
		}
		case "BodyOriVelLoadSensor"_hash:
		{
			const auto body_name = pn.get<String>( "body" );
			const auto postfix = pn.get<String>( "postfix" );
			const auto& body = *FindByName( model.GetBodies(), body_name );
			auto kv = par.try_get( body_name + postfix + ".KV", pn, "velocity_gain", 0.1 );
			const auto load_gain = pn.get<double>( "load_gain", 1.0 );
			const auto target = par.try_get( body_name + postfix + ".P0", pn, "target", 0 );;
			const auto delay = pn.get<double>( "delay" );
			for ( auto side : { RightSide, LeftSide } )
			{
				auto& bov = model.AcquireSensor<BodyOriVelSensor>( body, pn.get<Vec3>( "dir" ), kv, postfix, side, target );
				auto& load = model.AcquireSensor<LegLoadSensor>( model.GetLeg( Location( side ) ) );
				AddSensor( &model.AcquireDelayedSensor<ModulatedSensor>( bov, load, load_gain, 0, bov.GetName() + "ST" ), delay, 0 );
				AddSensor( &model.AcquireDelayedSensor<ModulatedSensor>( bov, load, -load_gain, 1, bov.GetName() + "SW" ), delay, 0 );
			}
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
				AddSensor( &model.AcquireDelayedSensor<DofPosVelSensor>( dof, kv, parent_dof, RightSide ), neural_delays_[ dof_name ], 0 );
				AddSensor( &model.AcquireDelayedSensor<DofPosVelSensor>( dof, kv, parent_dof, LeftSide ), neural_delays_[ dof_name ], 0 );
			}
			else AddSensor( &model.AcquireDelayedSensor<DofPosVelSensor>( dof, kv, parent_dof ), neural_delays_[ dof_name ], 0 );
			break;
		}
		case "LegLoadSensors"_hash:
		{
			for ( const auto& leg : model.GetLegs() )
				AddSensor( &model.AcquireDelayedSensor<LegLoadSensor>( *leg ), pn.get<double>( "delay" ), 0 );
			break;
		}
		case "InterNeurons"_hash:
		{
			auto& layer = AddNeuronLayer( pn, "leaky_relu" );
			const bool symmetric = pn.get<bool>( "symmetric", symmetric_ );

			// set names of interneurons
			auto& neuron_names = layer.names_;
			if ( auto neurons = pn.try_get<index_t>( "neurons" ) )
			{
				neuron_names.reserve( neuron_names.size() + *neurons * 2 );
				for ( auto s : { RightSide, LeftSide } )
					for ( index_t idx = 0; idx < *neurons; ++idx )
						neuron_names.emplace_back( xo::stringf( "I%d_%d_%c", layer.layer_idx_, idx, s == LeftSide ? 'l' : 'r' ) );
			}
			else if ( const auto names = pn.try_get<String>( "names" ) )
			{
				auto base_names = xo::split_str( *names, " ;," );
				neuron_names.reserve( neuron_names.size() + base_names.size() * 2 );
				for ( auto s : { RightSide, LeftSide } )
					for ( const auto& name : base_names )
						neuron_names.emplace_back( name + GetSideName( s ) );
			}

			// neuron names are set at this point, and used for counting
			const auto neurons = neuron_names.size();
			const auto& offset = pn.get_child( "offset" );
			auto start_idx = layer.neurons_.size();
			layer.neurons_.resize( neurons );
			for ( index_t idx = start_idx; idx < neurons; ++idx )
			{
				// we can use a const ref here because interneuron names are always stored internally
				const String& neuronname = neuron_names[ idx ];
				String parname = ( symmetric ? GetNameNoSide( neuronname ) : neuronname ) + ".C0";
				layer.neurons_[ idx ].offset_ = par.get( parname, offset );
			}
			break;
		}
		case "MotorNeurons"_hash:
		{
			SCONE_ERROR_IF( motor_layer_ != no_index, "MotorNeurons already defined" );
			auto& layer = AddNeuronLayer( pn, "relu" );
			motor_layer_ = layer.layer_idx_;
			const bool ignore_muscle_lines = pn.get<bool>( "ignore_muscle_lines", false ); // defaults to false for back comp
			const bool symmetric = pn.get<bool>( "symmetric", symmetric_ );
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
		case "RenshawNeurons"_hash:
		{
			SCONE_ERROR_IF( motor_layer_ == no_index, "No MotorNeurons defined" );
			auto& rs_layer = AddNeuronLayer( pn, "leaky_relu" );
			auto& in_links = AddLinkLayer( motor_layer_, rs_layer.layer_idx_ );
			auto& out_links = AddLinkLayer( rs_layer.layer_idx_, motor_layer_ );
			auto& mn_layer = layers_[ motor_layer_ ];

			const bool ignore_muscle_lines = pn.get<bool>( "ignore_muscle_lines", ignore_muscle_lines_ );
			const bool symmetric = pn.get<bool>( "symmetric", symmetric_ );

			// add RS neurons
			for ( auto idx : xo::size_range( mn_layer.neurons_ ) )
			{
				const auto& mus_name = mn_layer.names_[ idx ];
				auto par_name = GetParName( mus_name, ignore_muscle_lines, symmetric ) + ".RS0";
				rs_layer.neurons_.emplace_back( par.get( par_name, pn.get_child( "offset" ) ) );
				rs_layer.names_.emplace_back( mus_name + ".RS" );
			}

			// add links
			for ( auto idx : xo::size_range( mn_layer.neurons_ ) )
			{
				const auto& mus_name = mn_layer.names_[ idx ];
				in_links.links_.push_back( Link{ idx, idx, 1.0 } ); // input weights are always 1
				auto out_par_name = GetParName( mus_name, mus_name, "RS", ignore_muscle_lines, symmetric );
				double out_weight = par.get( out_par_name, pn.get_child( "weight" ) );
				out_links.links_.push_back( Link{ idx, idx, out_weight } );
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
		auto output_layer_idx = pn.get<index_t>( "output_layer", motor_layer_ );
		SCONE_ASSERT( input_layer_idx < layers_.size() && output_layer_idx < layers_.size() );
		auto& link_layer = AddLinkLayer( input_layer_idx, output_layer_idx );
		bool sensor_motor_link = input_layer_idx == 0 && output_layer_idx == layers_.size() - 1;
		const bool ignore_muscle_lines = pn.get<bool>( "ignore_muscle_lines", ignore_muscle_lines_ );
		const bool symmetric = pn.get<bool>( "symmetric", symmetric_ );
		auto input_include = pn.try_get_any<xo::pattern_matcher>( { "input_include", "input" } );
		auto output_include = pn.try_get_any<xo::pattern_matcher>( { "output_include", "output" } );
		auto input_type = pn.try_get<String>( "type" );

		const auto contralateral = pn.get<bool>( "contralateral", false );
		const auto ipsilateral = pn.get<bool>( "ipsilateral", !contralateral );

		const auto same_name = pn.get<bool>( "same_name", false );
		const auto normalize = pn.get<bool>( "normalize", false );
		auto begin_link = link_layer.links_.size();

		xo::flat_map<index_t, size_t> target_link_count;
		for ( auto target_neuron_idx : xo::irange( layers_[ output_layer_idx ].neurons_.size() ) )
		{
			const auto& target_name = GetNeuronName( output_layer_idx, target_neuron_idx );
			if ( output_include && !output_include->match( target_name ) )
				continue; // skip, not part of output pattern

			for ( auto source_neuron_idx : xo::irange( layers_[ input_layer_idx ].neurons_.size() ) )
			{
				const auto& source_name_full = GetNeuronName( input_layer_idx, source_neuron_idx );
				if ( input_include && !input_include->match( source_name_full ) )
					continue; // skip, not part of input pattern

				auto [source_name, source_type] = xo::split_str_at_last( source_name_full, "." );
				if ( input_type && source_type != *input_type )
					continue; // skip, wrong type

				auto src_side = GetSideFromName( source_name );
				auto trg_side = GetSideFromName( target_name );

				if ( ipsilateral && !src_side == NoSide && src_side != trg_side )
					continue; // neuron not on same side
				if ( contralateral && src_side == trg_side )
					continue; // neuron not on opposite side

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
				if ( normalize )
					++target_link_count[ target_neuron_idx ];
			}
		}
		auto end_link = link_layer.links_.size();
		if ( normalize )
			for ( auto idx = begin_link; idx < end_link; ++idx )
			{
				auto& link = link_layer.links_[ idx ];
				link.weight_ /= target_link_count[ link.trg_idx_ ];
			}
	}
}

#pragma once

#include "Controller.h"
#include "xo/utility/handle.h"
#include "xo/container/handle_vector.h"
#include <functional>
#include "xo/utility/hash.h"

namespace scone
{
	namespace NN
	{
		struct Neuron {
			Neuron() : input_(), offset_(), output_() {}
			Neuron( double offset ) : input_(), offset_( offset ), output_() {}
			double input_;
			double offset_;
			double output_;
		};

		using UpdateFunction = std::function<void( std::vector<Neuron>& )>;

		struct NeuronLayer : public std::vector<Neuron> {
			void update_outputs() { update_func_( *this ); }
			UpdateFunction update_func_;
		};

		inline double relu( const double v ) { return std::max( 0.0, v ); }
		inline double leaky_relu( const double v ) { return v >= 0.0 ? v : 0.01 * v; }
		inline double tanh( const double v ) { return std::tanh( v ); }
		inline double tanh_norm( const double v ) { return 0.5 * std::tanh( 2.0 * v - 1.0 ) + 0.5; }
		inline double tanh_norm_01( const double v ) { return 0.495 * std::tanh( 2.0 * v - 1.0 ) + 0.505; }

		// #perf #todo: check in compiler explorer if this inlines properly
		template< typename F >
		UpdateFunction update_function( F act ) {
			return [=]( std::vector<Neuron>& nv ) { for ( auto& n : nv ) n.output_ = act( n.input_ + n.offset_ ); };
		}

		inline UpdateFunction make_update_function( const String& s ) {
			switch ( xo::hash( s ) )
			{
			case "relu"_hash: return update_function( relu );
			case "leaky_relu"_hash: return update_function( leaky_relu );
			case "tanh"_hash: return update_function( tanh );
			case "tanh_norm"_hash: return update_function( tanh_norm );
			case "tanh_norm_01"_hash: return update_function( tanh_norm_01 );
			default: SCONE_ERROR( "Unknown activation function: " + s );
			}
		}

		struct Link {
			index_t src_idx_;
			index_t trg_idx_;
			double weight_ = 0;
		};
		struct LinkLayer
		{
			LinkLayer( index_t input_layer ) : input_layer_( input_layer ) {}
			index_t input_layer_;
			std::vector<Link> links_;
		};

		struct SensorNeuronLink {
			SensorDelayAdapter* sensor_;
			TimeInSeconds delay_;
			index_t neuron_idx_;
			const Muscle* muscle_;
		};

		struct MotorNeuronLink {
			Actuator* actuator_;
			index_t neuron_idx_;
			const Muscle* muscle_;
		};

		class NeuralNetworkController : public Controller
		{
		public:
			NeuralNetworkController( const PropNode& props, Params& par, Model& model, const Location& target_area );
			virtual ~NeuralNetworkController() {}

			void StoreData( Storage<Real>::Frame& frame, const StoreDataFlags& flags ) const override;
			PropNode GetInfo() const;

			const double leakyness_;
			const bool ignore_muscle_lines_;

		protected:
			bool ComputeControls( Model& model, double timestamp ) override;
			String GetClassSignature() const override;

		private:
			NeuronLayer& AddNeuronLayer( index_t layer );
			LinkLayer& AddLinkLayer( index_t input_layer, index_t output_layer );
			Neuron& AddSensor( SensorDelayAdapter* sensor, TimeInSeconds delay, double offset );
			Neuron& AddActuator( Actuator* actuator, double offset );
			const String& GetParAlias( const String& name );
			String GetParName( const String& name, bool ignore_muscle_lines );
			String GetParName( const String& target, const String& source, const String& type, bool ignore_muscle_lines );
			String GetNeuronName( index_t layer_idx, index_t neuron_idx ) const;

			void CreateLinkComponent( const PropNode& pn, Params& par, Model& model );
			void CreateComponent( const String& key, const PropNode& pn, Params& par, Model& model );

			const xo::flat_map<String, TimeInSeconds> neural_delays_;
			const xo::flat_map<String, String> parameter_aliases_;

			std::vector<SensorNeuronLink> sensor_links_;
			std::vector<NeuronLayer> neurons_;
			std::vector< std::vector< String > > neuron_names_;
			std::vector< std::vector< LinkLayer > > links_;
			std::vector<MotorNeuronLink> motor_links_;
		};
	}
}

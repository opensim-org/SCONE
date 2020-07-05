#pragma once

#include "Controller.h"
#include "xo/utility/handle.h"
#include "xo/container/handle_vector.h"

namespace scone
{
	namespace NN
	{
		inline double relu( const double v ) { return std::max( 0.0, v ); }
		inline double leaky_relu( const double v, const double l ) { return v >= 0.0 ? v : l * v; }

		struct Neuron {
			Neuron() : input_(), offset_(), output_() {}
			Neuron( double offset ) : input_(), offset_( offset ), output_() {}
			double input_;
			double offset_;
			double output_;
		};
		using NeuronLayer = std::vector<Neuron>;

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
			double offset_;
			double sign_;
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

		protected:
			bool ComputeControls( Model& model, double timestamp ) override;
			String GetClassSignature() const override;

		private:
			inline void update_output( Neuron& n ) { n.output_ = leaky_relu( n.input_ + n.offset_, leakyness_ ); }

			NeuronLayer& AddNeuronLayer( index_t layer );
			LinkLayer& AddLinkLayer( index_t input_layer, index_t output_layer );
			Neuron& AddSensor( SensorDelayAdapter* sensor, TimeInSeconds delay, double offset );
			Neuron& AddActuator( Actuator* actuator, double offset );
			String GetParName( const String& target, const String& source, const String& type, bool use_muscle_lines );
			String GetNeuronName( index_t layer_idx, index_t neuron_idx );

			void CreateLinkComponent( const PropNode& pn, Params& par, Model& model );
			void CreateComponent( const String& key, const PropNode& pn, Params& par, Model& model );

			const xo::flat_map< String, TimeInSeconds > neural_delays_;

			std::vector<SensorNeuronLink> sensor_links_;
			std::vector< NeuronLayer > neurons_;
			std::vector< std::vector< String > > neuron_names_;
			std::vector< std::vector< LinkLayer > > links_;
			std::vector<MotorNeuronLink> motor_links_;
		};
	}
}

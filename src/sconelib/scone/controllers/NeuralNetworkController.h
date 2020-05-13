#pragma once

#include "Controller.h"
#include "xo/utility/handle.h"
#include "xo/container/handle_vector.h"

namespace scone
{
	namespace NN
	{
		struct Neuron {
			double input_ = 0;
			double offset_ = 0;
			double output_ = 0;
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

		inline void update_output( Neuron& n ) { n.output_ = std::max( 0.0, n.input_ + n.offset_ ); }

		class NeuralNetworkController : public Controller
		{
		public:
			NeuralNetworkController( const PropNode& props, Params& par, Model& model, const Location& target_area );
			virtual ~NeuralNetworkController() {}

			void StoreData( Storage<Real>::Frame& frame, const StoreDataFlags& flags ) const override;
			PropNode GetInfo() const;

		protected:
			bool ComputeControls( Model& model, double timestamp ) override;
			String GetClassSignature() const override;

		private:

			NeuronLayer& AddNeuronLayer( index_t layer );
			LinkLayer& AddLinkLayer( index_t input_layer, index_t output_layer );
			Neuron& AddSensor( SensorDelayAdapter* sensor, TimeInSeconds delay, double offset );
			Neuron& AddActuator( Actuator* actuator );
			String GetParName( const String& target, const String& source, const String& type );
			void CreateComponent( const String& key, const PropNode& pn, Params& par, Model& model );

			const xo::flat_map< String, TimeInSeconds > neural_delays_;
			std::vector<SensorNeuronLink> sensor_links_;
			std::vector< NeuronLayer > neurons_;
			std::vector< std::vector< LinkLayer > > links_;
			std::vector<MotorNeuronLink> motor_links_;
		};
	}
}

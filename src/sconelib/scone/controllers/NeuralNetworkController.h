#pragma once

#include "Controller.h"
#include "xo/utility/handle.h"
#include "xo/container/handle_vector.h"
#include "xo/utility/hash.h"

namespace scone
{
	namespace NN
	{
		struct Neuron {
			Neuron() : input_(), offset_(), sum_(), output_() {}
			Neuron( double offset ) : input_(), offset_( offset ), sum_(), output_() {}
			double input_;
			double offset_;
			double sum_;
			double output_;
		};

		struct OutputUpdater {
			OutputUpdater( const PropNode& pn ) {}
			virtual ~OutputUpdater() = default;
			virtual void Update( std::vector<Neuron>& ) = 0;
		};

		template< typename F >
		struct BasicOutputUpdater : public OutputUpdater {
			BasicOutputUpdater( const PropNode& pn ) : OutputUpdater( pn ) {}
			virtual void Update( std::vector<Neuron>& nv ) override {
				for ( auto& n : nv )
					n.output_ = F::update( n.input_ + n.offset_ );
			}
		};

		template< typename F >
		struct DynamicOutputUpdater : public OutputUpdater {
			DynamicOutputUpdater( const PropNode& pn ) : OutputUpdater( pn ) {}
			DynamicOutputUpdater( double act_rate, double deact_rate, double dt ) {}
			virtual void Update( std::vector<Neuron>& nv ) override {
				for ( auto& n : nv ) {
					auto sum_new = n.input_ + n.offset_;
					auto sum_dot = ( sum_new - n.sum_ ) * ( c1_ * sum_new + c2_ );
					n.sum_ += dt_ * sum_dot;
					n.output_ = F::update( n.sum_ );
				}
			}
			double c1_, c2_, dt_;
		};

		struct NeuronLayer {
			std::vector<Neuron> neurons_;
			std::vector<String> names_;
			u_ptr<OutputUpdater> update_func_;
			void update_outputs() { update_func_->Update( neurons_ ); }
		};

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
			const bool symmetric_;

		protected:
			bool ComputeControls( Model& model, double timestamp ) override;
			String GetClassSignature() const override;

		private:
			NeuronLayer& AddNeuronLayer( index_t layer );
			LinkLayer& AddLinkLayer( index_t input_layer, index_t output_layer );
			Neuron& AddSensor( SensorDelayAdapter* sensor, TimeInSeconds delay, double offset );
			Neuron& AddActuator( Actuator* actuator, double offset );
			const String& GetParAlias( const String& name );
			String GetParName( const String& name, bool ignore_muscle_lines, bool symmetric );
			String GetParName( const String& target, const String& source, const String& type, bool ignore_muscle_lines, bool symmetric );
			String GetNeuronName( index_t layer_idx, index_t neuron_idx ) const;

			void CreateLinkComponent( const PropNode& pn, Params& par, Model& model );
			void CreateComponent( const String& key, const PropNode& pn, Params& par, Model& model );

			const xo::flat_map<String, TimeInSeconds> neural_delays_;
			const xo::flat_map<String, String> parameter_aliases_;

			std::vector<SensorNeuronLink> sensor_links_;
			std::vector<NeuronLayer> layers_;
			std::vector< std::vector< LinkLayer > > links_;
			std::vector<MotorNeuronLink> motor_links_;
			index_t motor_layer_ = no_index;
		};
	}
}

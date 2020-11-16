#pragma once

#include "Controller.h"
#include "xo/utility/handle.h"
#include "xo/container/handle_vector.h"
#include "xo/utility/hash.h"
#include "xo/container/circular_buffer.h"
#include <map>

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
			virtual void Update( std::vector<Neuron>&, const double dt ) = 0;
		};

		template< typename F >
		struct BasicOutputUpdater : public OutputUpdater {
			BasicOutputUpdater( const PropNode& pn ) : OutputUpdater( pn ) {}
			virtual void Update( std::vector<Neuron>& nv, const double dt ) override {
				for ( auto& n : nv )
					n.output_ = F::update( n.input_ + n.offset_ );
			}
		};

		template< typename F >
		struct DynamicOutputUpdater : public OutputUpdater {
			DynamicOutputUpdater( const PropNode& pn ) :
				OutputUpdater( pn ),
				deact_rate_( pn.get<float>( "deact_rate" ) ),
				act_rate_( pn.get<float>( "act_rate" ) )
			{}

			DynamicOutputUpdater( double act_rate, double deact_rate, double dt ) {}
			virtual void Update( std::vector<Neuron>& nv, const double dt ) override {
				for ( auto& n : nv ) {
					auto ds = n.input_ + n.offset_ - n.sum_;
					n.sum_ += dt * ds * ( ds > 0 ? act_rate_ : deact_rate_ );
					n.output_ = F::update( n.sum_ );
				}
			}
			double act_rate_, deact_rate_;
		};

		struct NeuronLayer {
			std::vector<Neuron> neurons_;
			std::vector<String> names_;
			u_ptr<OutputUpdater> update_func_;
			index_t layer_idx_ = no_index;
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

		using DelayBufferMap = std::map< size_t, xo::circular_buffer<Real> >;
		struct DelayBufferChannel {
			DelayBufferMap::iterator buffer_it_;
			index_t channel_idx_;
			void set( Real value ) const { buffer_it_->second.set( channel_idx_, value ); }
			Real get() const { return buffer_it_->second.get( channel_idx_ ); }
		};


		struct SensorNeuronLink {
			SensorDelayAdapter* delayed_sensor_;
			Sensor* sensor_;
			TimeInSeconds delay_;
			index_t neuron_idx_;
			const Muscle* muscle_;
			DelayBufferChannel buffer_channel_;
		};

		struct MotorNeuronLink {
			Actuator* actuator_;
			index_t neuron_idx_;
			const Muscle* muscle_;
			DelayBufferChannel buffer_channel_;
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
			const bool accurate_neural_delays_;

		protected:
			bool ComputeControls( Model& model, double timestamp ) override;
			String GetClassSignature() const override;

		private:
			NeuronLayer& AddNeuronLayer( const PropNode& pn, const String& default_activation );
			LinkLayer& AddLinkLayer( index_t input_layer, index_t output_layer );
			Neuron& AddSensor( Model& model, Sensor& sensor, TimeInSeconds delay, double offset );
			Neuron& AddActuator( const Model& model, Actuator& actuator, TimeInSeconds delay, double offset );
			const String& GetParAlias( const String& name );
			String GetParName( const String& name, bool ignore_muscle_lines, bool symmetric );
			String GetParName( const String& target, const String& source, const String& type, bool ignore_muscle_lines, bool symmetric );
			const String& GetNeuronName( index_t layer_idx, index_t neuron_idx ) const;

			void CreateLinkComponent( const PropNode& pn, Params& par, Model& model );
			void CreateComponent( const String& key, const PropNode& pn, Params& par, Model& model );

			const xo::flat_map<String, TimeInSeconds> neural_delays_;
			const xo::flat_map<String, String> parameter_aliases_;

			std::vector<SensorNeuronLink> sensor_links_;
			std::vector<NeuronLayer> layers_;
			std::vector< std::vector< LinkLayer > > links_;
			std::vector<MotorNeuronLink> motor_links_;
			index_t motor_layer_ = no_index;

			DelayBufferMap sensor_buffers_;
			DelayBufferMap actuator_buffers_;
		};
	}
}

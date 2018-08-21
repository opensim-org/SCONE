#pragma once

#include "scone/core/types.h"
#include "scone/core/PropNode.h"
#include "scone/model/Controller.h"
#include "xo/utility/types.h"
#include "MotorNeuron.h"
#include "SensorNeuron.h"
#include "InterNeuron.h"
#include "PatternNeuron.h"
#include "activation_functions.h"
#include "xo/container/flat_map.h"
#include "xo/string/string_tools.h"
#include "xo/utility/memoize.h"

namespace scone
{
	SCONE_DECLARE_STRUCT_AND_PTR( Neuron );
	SCONE_DECLARE_STRUCT_AND_PTR( InterNeuron );
	SCONE_DECLARE_STRUCT_AND_PTR( SensorNeuron );
	SCONE_DECLARE_STRUCT_AND_PTR( MotorNeuron );
	SCONE_DECLARE_STRUCT_AND_PTR( PatternNeuron );

	class NeuralController : public Controller
	{
	public:
		NeuralController( const PropNode& props, Params& par, Model& model, const Locality& target_area );
		virtual ~NeuralController() {}

		size_t GetLayerSize( const string& layer ) const { return ( layer == "0" ) ? m_SensorNeurons.size() : m_InterNeurons[ layer ].size(); }
		Neuron* GetNeuron( const string& layer, index_t idx ) { return ( layer == "0" ) ? dynamic_cast< Neuron* >( m_SensorNeurons[ idx ].get() ) : dynamic_cast< Neuron* >( m_InterNeurons[ layer ][ idx ].get() ); }

		const Model& GetModel() const { return model_; }
		Model& GetModel() { return model_; }

		std::vector< SensorNeuronUP >& GetSensorNeurons() { return m_SensorNeurons; }

		virtual bool ComputeControls( Model& model, double timestamp ) override;
		virtual void StoreData( Storage<Real>::Frame& frame, const StoreDataFlags& flags ) const override;

		virtual void WriteResult( const xo::path& file ) const override;

		static string FixLayerName( string str ) { return xo::from_str< int >( str ) > 0 ? "N" + str : str; }
		TimeInSeconds GetDelay( const string& name );

		struct MuscleParam {
			string name;
			double correlation;
			std::vector< Dof* > dofs;
		};

		using MuscleParamList = std::vector< MuscleParam >;
		enum parameter_mode_t { muscle_mode, dof_mode, virtual_mode, virtual_dof_mode };
		parameter_mode_t GetParMode() const { return par_mode_; }
		MuscleParamList GetMuscleParams( const Muscle* mus, bool is_sensor, bool apply_mirrorring ) const;
		MuscleParamList GetMuscleDofs( const Muscle* mus ) const;
		MuscleParamList GetVirtualMuscles( const Muscle* mus, bool apply_mirrorring ) const;

		double GetSimilarity( const NeuralController& other ) const;

		static bool IsMirrorDof( const Dof& dof );

		double min_virtual_muscle_correlation;

	protected:
		virtual String GetClassSignature() const override;

	private:
		Model& model_;
		PropNode delays_;
		TimeInSeconds delay_factor_;
		activation_func_t activation_function_;

		parameter_mode_t par_mode_;
		bool use_neutral_pose_;

		void AddSensorNeuronLayer( const PropNode& pn, Params& par );
		void AddPatternNeurons( const PropNode& pn, Params& par );
		void AddInterNeuronLayer( const PropNode& pn, Params& par );
		void AddMotorNeuronLayer( const PropNode& pn, Params& par );

		std::vector< PatternNeuronUP > m_PatternNeurons;
		std::vector< SensorNeuronUP > m_SensorNeurons;
		xo::flat_map< string, std::vector< InterNeuronUP > > m_InterNeurons;
		std::vector< MotorNeuronUP > m_MotorNeurons;
		mutable xo::memoize< MuscleParamList( const Muscle*, bool ) > m_VirtualMusclesMemoize;

		static MuscleParamList GetVirtualMusclesRecursiveFunc( const Muscle* mus, index_t joint_idx, bool mirror_dofs );
		static MuscleParamList GetVirtualMusclesFunc( const Muscle* mus, bool mirror_dofs );
	};
}

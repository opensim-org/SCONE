#include "NeuralController.h"

#include <algorithm>
#include <numeric>

#include "xo/container/container_tools.h"
#include "xo/container/table.h"
#include "xo/string/dictionary.h"
#include "xo/string/pattern_matcher.h"
#include "xo/string/string_cast.h"
#include "xo/string/string_tools.h"
#include "xo/utility/hash.h"

#include "scone/core/HasName.h"
#include "scone/core/Profiler.h"
#include "scone/core/string_tools.h"
#include "scone/model/Dof.h"
#include "scone/model/Joint.h"
#include "scone/model/Locality.h"
#include "scone/model/Model.h"
#include "scone/model/Muscle.h"
#include "scone/model/Side.h"
#include "scone/model/model_tools.h"

#include "InterNeuron.h"
#include "PatternNeuron.h"
#include "SensorNeuron.h"
#include "activation_functions.h"

namespace scone
{
	NeuralController::NeuralController( const PropNode& pn, Params& par, Model& model, const Locality& locality ) :
	Controller( pn, par, model, locality ),
	model_( model ),
	m_VirtualMusclesMemoize( GetVirtualMusclesFunc )
	{
		SCONE_PROFILE_FUNCTION;

		INIT_PROP( pn, min_virtual_muscle_correlation, 0 );
		INIT_PROP( pn, use_neutral_pose_, false );

		try
		{
			auto delay_file = pn.get< path >( "delay_file", "" );
			if ( !delay_file.empty() )
			{
				delays_ = load_file( FindFile( delay_file ), "zml" );
				model.AddExternalResource( delay_file );
			}

			INIT_PROP( pn, delay_factor_, 1.0 );
			par_mode_ = xo::lookup< parameter_mode_t >( pn.get< string >( "par_mode", "muscle" ), {
				{ "muscle", muscle_mode },
				{ "dof", dof_mode },
				{ "virtual", virtual_mode },
				{ "virtual_dof", virtual_dof_mode },
			} );

			activation_function_ = GetActivationFunction( pn.get< string >( "activation", "rectifier" ) );

			// backup the current state and set all DOFs to zero
			State org_state = model.GetState();
			if ( use_neutral_pose_ )
				model.SetNeutralState();
			else model.SetNullState();

			// create sensor neuron layer
			AddSensorNeuronLayer( pn.get_child( "SensorNeuronLayer" ), par );

			// create inter neuron layers
			for ( auto& n : pn.select( "InterNeuronLayer" ) )
				AddInterNeuronLayer( n.second, par );

			// create motor neuron layer
			AddMotorNeuronLayer( pn.get_child( "MotorNeuronLayer" ), par );

			// restore original state
			model.SetState( org_state, 0.0 );
		}
		catch ( std::exception& e )
		{
			SCONE_THROW( string( "Could not create NeuralController: " ) + e.what() );
		}
	}

	void NeuralController::AddSensorNeuronLayer( const PropNode& layer_pn, Params& par )
	{
		SCONE_PROFILE_FUNCTION;

		for ( auto& child_kvp : layer_pn )
		{
			auto& child_pn = child_kvp.second;

			auto type = child_pn.get< string >( "type" );
			auto source_mask = child_pn.get< string >( "source" );
			auto exclude_mask = child_pn.get< string >( "exclude", "" );
			std::vector< string > source_names;

			if ( type == "L" || type == "F" || type == "V" || type == "S" || type == "U" )
			{
				source_names = FindMatchingNames( GetModel().GetMuscles(), source_mask, exclude_mask );
				for ( auto& name : source_names )
					m_SensorNeurons.emplace_back( std::make_unique< SensorNeuron >( child_pn, par, *this, name, m_SensorNeurons.size(), GetSideFromName( name ), "linear" ) );
			}
			else if ( type == "DP" || type == "DV" || type == "DPV" )
			{
				source_names = FindMatchingNames( GetModel().GetDofs(), source_mask, exclude_mask );
				SCONE_THROW_IF( source_names.empty(), "Could not find any DOF matching " + xo::quoted( source_mask ) + " excluding " + xo::quoted( exclude_mask ) );
				for ( auto& name : source_names )
				{
					m_SensorNeurons.emplace_back( std::make_unique< SensorNeuron >( child_pn, par, *this, name, m_SensorNeurons.size(), LeftSide, "linear" ) );
					m_SensorNeurons.emplace_back( std::make_unique< SensorNeuron >( child_pn, par, *this, name, m_SensorNeurons.size(), RightSide, "linear" ) );
				}
			}
		}
	}

	void NeuralController::AddPatternNeurons( const PropNode& pn, Params& par )
	{
		SCONE_PROFILE_FUNCTION;

		auto amount = pn.get< int >( "amount" );
		for ( int i = 0; i < amount; ++i )
			m_PatternNeurons.emplace_back( std::make_unique< PatternNeuron >( pn, par, *this, i, false ) );
		for ( int i = 0; i < amount; ++i )
			m_PatternNeurons.emplace_back( std::make_unique< PatternNeuron >( pn, par, *this, i, true ) );
	}

	void NeuralController::AddInterNeuronLayer( const PropNode& pn, Params& par )
	{
		SCONE_PROFILE_FUNCTION;

		auto layer_name = FixLayerName( pn.get< string >( "layer" ) );
		int amount = pn.get< int >( "neurons" );
		string act_func = pn.get< string >( "activation", "rectifier" );

		auto& layer = m_InterNeurons[ layer_name ];
		for ( int i = 0; i < amount; ++i )
		{
			for ( auto side : { LeftSide, RightSide } )
			{
				layer.emplace_back( std::make_unique< InterNeuron >( pn, par, layer_name, i, side, act_func ) );
				for ( auto& child : pn )
					layer.back()->AddInputs( child.second, par, *this );
			}
		}
	}

	void NeuralController::AddMotorNeuronLayer( const PropNode& pn, Params& par )
	{
		SCONE_PROFILE_FUNCTION;

		for ( auto& muscle : GetModel().GetMuscles() )
		{
			auto name = muscle->GetName();
			m_MotorNeurons.emplace_back( std::make_unique< MotorNeuron >( pn, par, *this, name, m_MotorNeurons.size(), GetSideFromName( name ) ) );
			for ( auto& child_pn : pn )
			{
				auto include = child_pn.second.get< xo::pattern_matcher >( "include", "*" );
				auto exclude = child_pn.second.get< xo::pattern_matcher >( "exclude", "" );
				if ( include( name ) && !exclude( name ) )
					m_MotorNeurons.back()->AddInputs( child_pn.second, par, *this );
			}
		}
	}

	scone::NeuralController::MuscleParamList NeuralController::GetVirtualMusclesRecursiveFunc( const Muscle* mus, index_t joint_idx, bool apply_mirroring )
	{
		auto& joints = mus->GetJoints();
		if ( joint_idx >= joints.size() )
			return MuscleParamList();

		auto children = GetVirtualMusclesRecursiveFunc( mus, joint_idx + 1, apply_mirroring );

		auto& joint = joints[ joint_idx ];
		auto& dofs = joint->GetDofs();
		if ( dofs.empty() )
			return children;

		MuscleParamList results;
		for ( index_t dof_idx = 0; dof_idx < dofs.size(); ++dof_idx )
		{
			auto& dof = dofs[ dof_idx ];
			auto mom = mus->GetNormalizedMomentArm( *dof );
			if ( mom != 0 )
			{
				// swap the sign if this is a "mirror dof" (e.g. lumbar_bending) AND mirroring should be applied
				if ( apply_mirroring && IsMirrorDof( *dof ) )
					mom = -mom;
				auto name = GetNameNoSide( dof->GetName() ) + GetSignChar( mom );
				if ( !children.empty() )
				{
					for ( auto& ch : children )
					{
						results.push_back( { name + ch.name, abs( mom ) * ch.correlation, ch.dofs } );
						results.back().dofs.push_back( dof );
					}
				}
				else results.push_back( { name, abs( mom ), { dof } } );
			}
		}

		return results;
	}

	scone::NeuralController::MuscleParamList NeuralController::GetVirtualMusclesFunc( const Muscle* mus, bool mirror_dofs )
	{
		SCONE_PROFILE_FUNCTION;

		auto result = GetVirtualMusclesRecursiveFunc( mus, 0, mirror_dofs );

		// square root & normalize
		double total_gain = 0.0;
		for ( auto& vm : result )
			total_gain += ( vm.correlation = sqrt( vm.correlation ) );

		for ( auto& vm : result )
			vm.correlation /= total_gain;

		return result;
	}

	scone::NeuralController::MuscleParamList NeuralController::GetVirtualMuscles( const Muscle* mus, bool mirror_dofs ) const
	{
		return m_VirtualMusclesMemoize( mus, mirror_dofs );
	}

	scone::NeuralController::MuscleParamList NeuralController::GetMuscleDofs( const Muscle* mus ) const
	{
		MuscleParamList result;

		for ( auto& dof : mus->GetModel().GetDofs() )
		{
			if ( mus->HasMomentArm( *dof ) )
			{
				auto mom = mus->GetNormalizedMomentArm( *dof );
				result.push_back( { GetNameNoSide( dof->GetName() ) + GetSignChar( mom ), abs( mom ),{ dof.get() } } );
			}
		}
		return result;
	}

	bool NeuralController::ComputeControls( Model& model, double timestamp )
	{
		SCONE_PROFILE_FUNCTION;

		for ( auto& n : m_MotorNeurons )
			n->UpdateActuator();

		return false;
	}

	void NeuralController::StoreData( Storage<Real>::Frame& frame, const StoreDataFlags& flags ) const
	{
		for ( auto& neuron : m_PatternNeurons )
			frame[ "PN." + neuron->GetName( false ) ] = neuron->output_;
		for ( auto& neuron : m_SensorNeurons )
			frame[ "SN." + neuron->GetName( false ) ] = neuron->output_;
		for ( auto& layer : m_InterNeurons )
			for ( auto& neuron : layer.second )
				frame[ "IN." + neuron->GetName( false ) ] = neuron->output_;
		for ( auto& neuron : m_MotorNeurons )
		{
			auto prefix = "MN." + neuron->GetName( false ) + '.';
			frame[ prefix + "input" ] = neuron->input_;
			for ( auto& i : neuron->inputs_ )
				frame[ prefix + i.neuron->GetName( false ) ] = i.gain * i.neuron->GetOutput();
		}
	}

	void NeuralController::WriteResult( const path& file ) const
	{
		xo::table< double > weights, contribs, sources;
		std::vector< std::pair< double, string > > contrib_vec;

		for ( auto& inter_layer : m_InterNeurons )
		{
			for ( auto& neuron : inter_layer.second )
			{
				for ( auto& input : neuron->inputs_ )
					weights( input.neuron->GetName( false ), neuron->name_ ) = input.gain;
			}
		}

		for ( auto& neuron : m_MotorNeurons )
		{
			auto tot = std::accumulate( neuron->inputs_.begin(), neuron->inputs_.end(), 0.0, [&]( double v1, const InterNeuron::Input& i ) { return v1 + i.contribution; } );
			for ( auto& input : neuron->inputs_ )
			{
				weights( input.neuron->GetName( false ), neuron->name_ ) = input.gain;
				contribs( input.neuron->GetName( false ), neuron->name_ ) = input.contribution / tot;
				sources( xo::left_of_str( input.neuron->GetParName(), "." ), neuron->GetParName() ) += input.contribution / tot / 2.0;
				contrib_vec.emplace_back( input.contribution, neuron->name_ + "\t" + input.neuron->GetName( false ) );
			}
		}

		std::sort( contrib_vec.begin(), contrib_vec.end(), std::greater<>() );

		std::ofstream str( ( file + ".NeuralController.txt" ).str() );

		// output gains
		str << weights << std::endl;

		// output input contributions
		str << contribs << std::endl;
		for ( auto& c : contrib_vec )
			str << c.second << "\t" << c.first << std::endl;
		str << std::endl;

		// output sources
		str << sources << std::endl;

		// output virtual muscles
		for ( auto& m : GetModel().GetMuscles() )
		{
			str << m->GetName();
			auto mp = GetVirtualMuscles( m.get(), GetSideFromName( m->GetName() ) == RightSide );
			for ( auto& par : mp )
				str << "\t" << par.name << "\t" << par.correlation;
			str << std::endl;
		}

	}

	String NeuralController::GetClassSignature() const
	{
		size_t c = 0;
		for ( auto& layer : m_InterNeurons )
			for ( auto& neuron : layer.second )
				c += neuron->GetInputs().size();

		for ( auto& neuron : m_MotorNeurons )
			c += neuron->GetInputs().size();

		return xo::stringf( "N%d", c );
	}

	TimeInSeconds NeuralController::GetDelay( const string& name )
	{
		return delay_factor_ * delays_.get< double >( name );
	}

	NeuralController::MuscleParamList NeuralController::GetMuscleParams( const Muscle* mus, bool is_sensor, bool apply_mirrorring ) const
	{
		SCONE_PROFILE_FUNCTION;

		if ( mus )
		{
			switch ( par_mode_ )
			{
			case NeuralController::muscle_mode: return { { GetNameNoSide( mus->GetName() ), 1, {} } };
			case NeuralController::dof_mode: return GetMuscleDofs( mus );
			case NeuralController::virtual_mode: return GetVirtualMuscles( mus, apply_mirrorring );
			case NeuralController::virtual_dof_mode: return is_sensor ? GetMuscleDofs( mus ) : GetVirtualMuscles( mus, apply_mirrorring );
			default: SCONE_THROW( "Unknown parameter mode" );
			}
		}
		else return { { "IN", 1 } };
	}

	double NeuralController::GetSimilarity( const NeuralController& other ) const
	{
		SCONE_ASSERT( m_MotorNeurons.size() == other.m_MotorNeurons.size() );

		double fitness = 0.0;
		int samples = 0;
		for ( auto& neuron : m_MotorNeurons )
		{
			auto other_neuron = xo::find_if( other.m_MotorNeurons, [&]( const MotorNeuronUP& m ) { return neuron->GetName() == m->GetName(); } );
			SCONE_THROW_IF( other_neuron == other.m_MotorNeurons.end(), "Could not find Neuron " + neuron->GetName() );

			// measure difference in MotorNeuron offset
			fitness += abs( neuron->offset_ - (*other_neuron)->offset_ );
			++samples;

			for ( auto& input : neuron->GetInputs() )
			{
				auto other_input = xo::find_if( (*other_neuron)->GetInputs(), [&]( const Neuron::Input& i ) { return input.neuron->GetName() == i.neuron->GetName(); } );
				SCONE_THROW_IF( other_input == (*other_neuron )->GetInputs().end(), "Could not find Input " + input.neuron->GetName() + " for " + neuron->GetName() );

				// measure difference in MotorNeuron input gain
				fitness += abs( input.gain - other_input->gain );
				++samples;
			}
		}
		return 100 * fitness / samples;
	}

	bool NeuralController::IsMirrorDof( const Dof& dof )
	{
		return xo::str_equals_any_of( dof.GetName(), { "lumbar_bending", "lumbar_rotation", "pelvis_list", "pelvis_rotation" } );
	}
}

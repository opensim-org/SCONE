#include "MetaReflexMuscle.h"
#include "scone/model/Model.h"
#include "scone/model/SensorDelayAdapter.h"
#include "scone/model/Dof.h"
#include "MetaReflexDof.h"
#include "MetaReflexController.h"
#include "SimTKcommon/internal/common.h"
#include "scone/core/Log.h"

//#define DEBUG_MUSCLE "iliopsoas_r"
//#define INFO_MUSCLE "glut_max_l"
//#define DEBUG_MUSCLE "rect_fem_l"

namespace scone
{
	namespace cs
	{
		MetaReflexMuscle::MetaReflexMuscle( sim::Muscle& mus, sim::Model& model, const MetaReflexController& controller, const sim::Area& area ) : 
		muscle( mus ),
		force_sensor( model.AcquireDelayedSensor< sim::MuscleForceSensor >( mus ) ),
		length_sensor( model.AcquireDelayedSensor< sim::MuscleLengthSensor >( mus ) ),
		length_gain( 0.0 ),
		constant( 0.0 ),
		force_gain( 0.0 ),
		delay( 0.0 ),
		stiffness( 0.0 ),
		total_abs_moment_arm( 0.0 ),
		total_vm_similarity( 0.0 )
		{
			//ref_length_base = ( muscle.GetLength() - muscle.GetTendonSlackLength() ) / muscle.GetOptimalFiberLength();
			ref_length_base = 1.0;

			// precompute number of dofs and total moment arm
			for ( const MetaReflexDofUP& mrdof: controller.GetReflexDofs() )
			{
				if ( muscle.HasMomentArm( mrdof->target_dof ) )
				{
					DofInfo di( *mrdof );

					di.moment_arm = muscle.GetMomentArm( mrdof->target_dof );
					total_abs_moment_arm += std::abs( di.moment_arm );

					// TODO: use Radian as input parameter
					di.dof.target_dof.SetVel( Radian( Degree( 1 ) ).value );
					di.lengthening_speed = muscle.GetVelocity();

					// TODO: use Radian as input parameter
					di.dof.target_dof.SetVel( Radian( 0 ).value );

					dof_infos.push_back( di );
				}
			}

			// now compute the max available moment for each dof
			for ( DofInfo& di: dof_infos )
			{
				Real mom_w = di.moment_arm / total_abs_moment_arm;
				di.w = mom_w;
				di.abs_w = std::abs( di.w );
				di.max_moment = di.abs_w * di.moment_arm * muscle.GetMaxIsometricForce();
				di.dof.AddAvailableMoment( di.max_moment );
			}

			// virtual muscles
			Real summed_muscle_moment_arms = 0;
			for ( const auto& dof : model.GetDofs() )
			{
				if ( muscle.HasMomentArm( *dof ) )
					summed_muscle_moment_arms += std::abs( muscle.GetMomentArm( *dof ) );
			}

			size_t max_articulation = 0;
			for( const MetaReflexVirtualMuscleUP& vm : controller.GetVirtualMuscles() )
			{
				Real s = vm->GetSimilarity( muscle, summed_muscle_moment_arms );
				if ( s > 0 )
				{
					vm_infos.push_back( VirtualMuscleInfo{ vm.get(), s } );
					total_vm_similarity += s;
					max_articulation = std::max( max_articulation, vm->GetDofCount() );
					log::TraceF( "%-20s%-32ssim=%.3f tot=%.3f", muscle.GetName().c_str(), vm->name.c_str(), s, total_vm_similarity );
				}
			}

			// prune 
			for ( auto iter = vm_infos.begin(); iter != vm_infos.end(); )
			{
				if ( iter->vm->GetDofCount() < max_articulation )
				{
					total_vm_similarity -= iter->similarity;
					log::TraceF( "%-20s%-32ssim=%.3f tot=%.3f", muscle.GetName().c_str(), iter->vm->name.c_str(), iter->similarity, total_vm_similarity );
					iter = vm_infos.erase( iter );
				}
				else ++iter;
			}
		}

		MetaReflexMuscle::~MetaReflexMuscle()
		{
		}

		void MetaReflexMuscle::UpdateMuscleControlParameters( bool debug /*= false */ )
{
			// initialize reference length
			ref_length = ref_length_base;
			length_gain = 0;
			constant = 0;
			force_gain = 0;
			delay = 0;

			// compute muscle feedback parameters
			for ( const VirtualMuscleInfo& vmi : vm_infos )
			{
				auto& vm = *vmi.vm;
				length_gain += vmi.similarity * vm.mrp.length_gain;
				force_gain += vmi.similarity * vm.mrp.force_gain;
				constant += vmi.similarity * vm.mrp.constant;
				constant += vmi.similarity * vm.bal_mrp.constant * vm.GetLocalBalance();

				// delay, average of all VMs
				// TODO: move away from here!
				delay += vmi.similarity * vm.delay / total_vm_similarity; // TODO: compute per muscle
			}

			// compute muscle feedback parameters
			for ( const DofInfo& di: dof_infos )
			{
				// TODO: store these in DofInfo
				const MetaReflexParams& dof_par = di.w > 0 ? di.dof.dof_pos : di.dof.dof_neg;
				const MetaReflexParams& bal_par = di.w > 0 ? di.dof.bal_pos : di.dof.bal_neg;

				ref_length += di.w * di.lengthening_speed * bal_par.ref_pos.value * di.dof.GetLocalBalance();

				length_gain += di.abs_w * dof_par.length_gain;
				length_gain += di.abs_w * bal_par.length_gain * di.dof.GetLocalBalance();

				force_gain += di.w * dof_par.force_gain;
				force_gain += di.w * bal_par.force_gain * di.dof.GetLocalBalance();

				constant += di.w * dof_par.constant;
				constant += di.w * bal_par.constant * di.dof.GetLocalBalance();

				// delay, average of all MetaMuscleDofs
				// TODO: move away from here!
				delay += ( 1.0 / dof_infos.size() ) * di.dof.delay; // TODO: compute per muscle
			}

			if ( debug )
				log::TraceF( "%-20sl=%.3f f=%.3f c=%.3f d=%.3f", muscle.GetName().c_str(), length_gain, force_gain, constant, delay );


#ifdef INFO_MUSCLE
				if ( muscle.GetName() == INFO_MUSCLE || strlen( INFO_MUSCLE ) == 0 )
				{
					Real lb = di.dof.GetLocalBalance();
					Real bc = di.dof.bal_par.constant * di.dof.GetLocalBalance();
					log::PeriodicTraceF( 20, "%-20s%-20sref=%6.3f c=%.3f (w=% .2f * (lb=%.3f * %.3f + %.3f))",
						muscle.GetName().c_str(), di.dof.target_dof.GetName().c_str(), 
						ref_length, constant, di.w, lb, di.dof.bal_par.constant, di.dof.dof_par.constant );
				}
#endif

		}

		Real MetaReflexMuscle::ComputeStiffnessExcitation( MetaReflexDof& dof )
		{
			SCONE_THROW_NOT_IMPLEMENTED;
			//Real mus_mom_arm = muscle.GetMomentArm( dof.target_dof );
			//Real max_mus_mom = mus_mom_arm * muscle.GetMaxIsometricForce();

			//Real max_abs_dof_mom = std::min( abs( dof.tot_available_neg_mom ), dof.tot_available_pos_mom );
			//Real des_dof_mom = dof.dof_par.stiffness * Sign( mus_mom_arm ) * max_abs_dof_mom;

			//Real max_stiffness_mom = ( mus_mom_arm < 0 ) ? dof.tot_available_neg_mom : dof.tot_available_pos_mom;
			//Real available_mus_mom = ( abs( mus_mom_arm ) / total_abs_moment_arm ) * max_mus_mom;
			//Real mus_mom_contrib = available_mus_mom / max_stiffness_mom;

			//Real des_mus_mom = mus_mom_contrib * des_dof_mom;
			//Real a = des_mus_mom / max_mus_mom;

			//return a;
		}

		void MetaReflexMuscle::UpdateControls()
		{
			// length feedback
			Real current_length = length_sensor.GetValue( delay );
			Real ul = length_gain * ( current_length - ref_length );

			// constant excitation
			Real uc = constant;

			// force feedback
			Real uf = force_gain * std::max( 0.0, force_sensor.GetValue( delay ) );

			// compute total
			Real total = ul + uc + uf;
			muscle.AddControlValue( std::max( 0.0, total ) );

#ifdef DEBUG_MUSCLE
			if ( muscle.GetName() == DEBUG_MUSCLE )
				log::PeriodicTraceF( 20, "%s: u=%.3f l=%.3f bl=%.3f lg=%.3f ul=%.3f uc=%.3f", DEBUG_MUSCLE, total, current_length, ref_length_base, length_gain, ul );
#endif
		}
	}
}

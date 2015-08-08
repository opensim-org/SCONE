#include "stdafx.h"

#include "MetaReflexMuscle.h"
#include "../sim/Model.h"
#include "../sim/SensorDelayAdapter.h"
#include "../sim/Dof.h"
#include "MetaReflexDof.h"
#include "boost/foreach.hpp"
#include "MetaReflexController.h"

//#define DEBUG_MUSCLE "iliopsoas_r"

namespace scone
{
	namespace cs
	{
		MetaReflexMuscle::MetaReflexMuscle( sim::Muscle& mus, sim::Model& model, const MetaReflexController& controller, const sim::Area& area ) : 
		muscle( mus ),
		force_sensor( model.AcquireDelayedSensor< sim::MuscleForceSensor >( mus ) ),
		length_sensor( model.AcquireDelayedSensor< sim::MuscleLengthSensor >( mus ) ),
		length_gain( 0.0 ),
		constant_ex( 0.0 ),
		force_gain( 0.0 ),
		delay( 0.0 ),
		stiffness( 0.0 ),
		dof_count( 0 ),
		total_abs_moment_arm( 0.0 )
		{
			ref_length = ( muscle.GetLength() - muscle.GetTendonSlackLength() ) / muscle.GetOptimalFiberLength();

			// precompute number of dofs and total moment arm
			BOOST_FOREACH( const MetaReflexDofUP& mrdof, controller.GetReflexDofs() )
			{
				if ( muscle.HasMomentArm( mrdof->target_dof ) )
				{
					dof_count++;
					Real mus_mom_arm = muscle.GetMomentArm( mrdof->target_dof );
					total_abs_moment_arm += abs( mus_mom_arm );
				}
			}

			// now compute the max available moment for each dof
			BOOST_FOREACH( const MetaReflexDofUP& mrdof, controller.GetReflexDofs() )
			if ( muscle.HasMomentArm( mrdof->target_dof ) )
			{
				Real mus_mom_arm = muscle.GetMomentArm( mrdof->target_dof );
				Real mus_dof_weight = abs( mus_mom_arm ) / total_abs_moment_arm;

				mrdof->AddAvailableMoment( mus_dof_weight * mus_mom_arm * muscle.GetMaxIsometricForce() );
			}
		}

		MetaReflexMuscle::~MetaReflexMuscle()
		{
		}

		void MetaReflexMuscle::InitMuscleParameters( const MetaReflexController& controller )
		{
			// compute muscle feedback parameters
			BOOST_FOREACH( const MetaReflexDofUP& mrdof, controller.GetReflexDofs() )
			{
				if ( muscle.HasMomentArm( mrdof->target_dof ) )
				{
					Real norm_moment_arm = muscle.GetMomentArm( mrdof->target_dof ) / total_abs_moment_arm;
					Real w = 1.0 / dof_count;

					length_gain += abs( norm_moment_arm ) * mrdof->length_gain;
					force_gain += norm_moment_arm * mrdof->force_feedback;
					constant_ex += norm_moment_arm * mrdof->constant;

					// stiffness
					if ( mrdof->stiffness > 0.0 )
						stiffness += ComputeStiffnessExcitation( *mrdof );

					// delay
					delay += w * mrdof->delay; // TODO: compute per muscle

					log::TraceF( "%-20s%-20sdof=% 6.1f len=%6.3f mom=% 8.3f se=%.3f",
						muscle.GetName().c_str(), mrdof->target_dof.GetName().c_str(), 
						mrdof->ref_pos_in_deg, ref_length, norm_moment_arm, stiffness );
				}
			}
		}

		Real MetaReflexMuscle::ComputeStiffnessExcitation( MetaReflexDof& dof )
		{
			Real mus_mom_arm = muscle.GetMomentArm( dof.target_dof );
			Real max_mus_mom = mus_mom_arm * muscle.GetMaxIsometricForce();

			Real max_abs_dof_mom = std::min( abs( dof.tot_available_neg_mom ), dof.tot_available_pos_mom );
			Real des_dof_mom = dof.stiffness * Sign( mus_mom_arm ) * max_abs_dof_mom;

			Real tot_available_dof_mom = ( mus_mom_arm < 0 ) ? dof.tot_available_neg_mom : dof.tot_available_pos_mom;
			Real available_mus_mom = ( abs( mus_mom_arm ) / total_abs_moment_arm ) * max_mus_mom;
			Real mus_mom_contrib = available_mus_mom / tot_available_dof_mom;

			Real des_mus_mom = mus_mom_contrib * des_dof_mom;
			Real a = des_mus_mom / max_mus_mom;

			return a;
		}

		void MetaReflexMuscle::UpdateControls()
		{
			// length feedback
			Real ul = length_gain * std::max( 0.0, length_sensor.GetValue( delay ) - ref_length );

			// constant excitation
			Real uc = std::max( 0.0, constant_ex + stiffness );

			// force feedback
			Real uf = force_gain * std::max( 0.0, force_sensor.GetValue( delay ) );

			// compute total
			Real total = ul + uc + uf;

			muscle.AddControlValue( total );

#ifdef DEBUG_MUSCLE
			if ( muscle.GetName() == DEBUG_MUSCLE )
				log::TraceF( "%s: l=%.3f ref_l=%.3f lg=%.3f ul=%.3f", DEBUG_MUSCLE, length_sensor.GetValue( delay ), ref_length, length_gain, ul );
#endif
		}
	}
}

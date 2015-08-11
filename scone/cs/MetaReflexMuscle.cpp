#include "stdafx.h"

#include "MetaReflexMuscle.h"
#include "../sim/Model.h"
#include "../sim/SensorDelayAdapter.h"
#include "../sim/Dof.h"
#include "MetaReflexDof.h"
#include "boost/foreach.hpp"
#include "MetaReflexController.h"
#include "tools.h"

//#define DEBUG_MUSCLE "iliopsoas_r"
#define INFO_MUSCLE "bifemlh_r"

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
		total_abs_moment_arm( 0.0 )
		{
			ref_length = ( muscle.GetLength() - muscle.GetTendonSlackLength() ) / muscle.GetOptimalFiberLength();

			// precompute number of dofs and total moment arm
			BOOST_FOREACH( const MetaReflexDofUP& mrdof, controller.GetReflexDofs() )
			{
				if ( muscle.HasMomentArm( mrdof->target_dof ) )
				{
					DofInfo di;
					di.dof = mrdof.get();
					di.moment_arm = muscle.GetMomentArm( mrdof->target_dof );
					dof_infos.push_back( di );

					total_abs_moment_arm += abs( di.moment_arm );
				}
			}

			// now compute the max available moment for each dof
			BOOST_FOREACH( DofInfo& di, dof_infos )
			{
				di.contrib_weight = abs( di.moment_arm ) / total_abs_moment_arm;
				di.max_moment = di.contrib_weight * di.moment_arm * muscle.GetMaxIsometricForce();
				di.dof->AddAvailableMoment( di.max_moment );
			}
		}

		MetaReflexMuscle::~MetaReflexMuscle()
		{
		}

		void MetaReflexMuscle::InitMuscleParameters( const MetaReflexController& controller )
		{
			// compute muscle feedback parameters
			BOOST_FOREACH( const DofInfo& di, dof_infos )
			{
				Real norm_moment_arm = di.moment_arm / total_abs_moment_arm;

				length_gain += abs( norm_moment_arm ) * di.dof->length_gain;
				force_gain += norm_moment_arm * di.dof->force_feedback;
				constant_ex += norm_moment_arm * di.dof->constant;

				// stiffness
				if ( di.dof->stiffness > 0.0 )
					stiffness += ComputeStiffnessExcitation( *di.dof );

				// delay, average of all MetaMuscleDofs
				delay += ( 1.0 / dof_infos.size() ) * di.dof->delay; // TODO: compute per muscle

#ifdef INFO_MUSCLE
				if ( muscle.GetName() == INFO_MUSCLE || strlen( INFO_MUSCLE ) == 0 )
				{
					log::TraceF( "%-20s%-20sdof=% 6.1f len=%6.3f mom=% 8.3f se=%.3f",
						muscle.GetName().c_str(), di.dof->target_dof.GetName().c_str(), 
						di.dof->ref_pos_in_deg, ref_length, norm_moment_arm, stiffness );
				}
#endif
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

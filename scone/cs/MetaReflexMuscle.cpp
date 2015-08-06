#include "stdafx.h"

#include "MetaReflexMuscle.h"
#include "../sim/Model.h"
#include "../sim/SensorDelayAdapter.h"
#include "../sim/Dof.h"
#include "MetaReflexDof.h"
#include "boost/foreach.hpp"

//#define DEBUG_MUSCLE "tib_ant_r"

namespace scone
{
	namespace cs
	{
		MetaReflexMuscle::MetaReflexMuscle( sim::Muscle& mus, sim::Model& model, const std::vector< MetaReflexDofUP >& mrdofs ) : 
		muscle( mus ),
		force_sensor( model.AcquireDelayedSensor< sim::MuscleForceSensor >( mus ) ),
		length_sensor( model.AcquireDelayedSensor< sim::MuscleLengthSensor >( mus ) ),
		length_gain( 0.0 ),
		constant_ex( 0.0 ),
		force_gain( 0.0 ),
		delay( 0.0 ),
		stiffness( 0.0 ),
		dof_count( 0 )
		{
			ref_length = ( muscle.GetLength() - muscle.GetTendonSlackLength() ) / muscle.GetOptimalFiberLength();

			// precompute number of dofs and total moment arm
			Real total_moment_arm = 0.0;
			BOOST_FOREACH( const MetaReflexDofUP& mrdof, mrdofs )
			{
				if ( muscle.HasMomentArm( mrdof->target_dof ) )
				{
					dof_count++;
					total_moment_arm += abs( muscle.GetMomentArm( mrdof->target_dof ) );
				}
			}

			// compute muscle feedback parameters
			BOOST_FOREACH( const MetaReflexDofUP& mrdof, mrdofs )
			{
				if ( muscle.HasMomentArm( mrdof->target_dof ) )
				{
					Real norm_moment_arm = muscle.GetMomentArm( mrdof->target_dof ) / total_moment_arm;
					Real w = 1.0 / dof_count;

					length_gain += w * mrdof->length_gain; // TODO: include moment arm?
					force_gain += w * norm_moment_arm * mrdof->force_feedback;
					constant_ex += w * norm_moment_arm * mrdof->constant;
					stiffness += w * mrdof->stiffness; // TODO: include moment arm?

					delay += w * mrdof->delay; // TODO: compute per muscle

					log::TraceF( "%-20s%-20sdof=% 6.1f len=% 8.3f mom=% 8.3f",
						muscle.GetName().c_str(), mrdof->target_dof.GetName().c_str(), 
						mrdof->ref_pos_in_deg, ref_length, norm_moment_arm );
				}
			}

		}

		MetaReflexMuscle::~MetaReflexMuscle()
		{
		}

		void MetaReflexMuscle::UpdateControls()
		{
			// length feedback
			Real ul = length_gain * std::max( 0.0, length_sensor.GetValue( delay ) - ref_length );

			// constant excitation
			Real uc = std::max( 0.0, constant_ex );

			// force feedback
			Real uf = force_gain * std::max( 0.0, force_sensor.GetValue( delay ) );

			muscle.AddControlValue( ul + uc + uf );

#ifdef DEBUG_MUSCLE
			if ( muscle.GetName() == DEBUG_MUSCLE )
				log::TraceF( "length=%.3f reference=%.3f ul=%.3f", length_sensor.GetValue( delay ), ref_length, ul );
#endif
		}
	}
}

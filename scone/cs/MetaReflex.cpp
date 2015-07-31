#include "stdafx.h"

#include <boost/foreach.hpp>

#include "MetaReflex.h"
#include "../sim/Area.h"
#include "../sim/Dof.h"
#include "../core/InitFromPropNode.h"
#include "../sim/Muscle.h"
#include "../sim/Link.h"
#include "../sim/Joint.h"
#include "../sim/SensorDelayAdapter.h"

namespace scone
{
	namespace cs
	{
		MetaReflex::MetaReflex( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area ) :
		target_dof( *FindByName( model.GetDofs(), props.GetStr( "target" ) ) )
		{
			INIT_PROPERTY( props, reference_pos, 0.0 );
			INIT_PROPERTY( props, length_gain, 0.0 );
			INIT_PROPERTY( props, constant_moment, 0.0 );
			INIT_PROPERTY( props, force_feedback, 0.0 );
			INIT_PROPERTY( props, delay, 0.0 );

			// create muscle infos
			BOOST_FOREACH( sim::MuscleUP& mus, model.GetMuscles() )
			{
				if ( MuscleCrossesDof( *mus ) )
				{
					// don't set moment_arm and length just yet
					// the pose first needs to be updated by MetaReflexController
					m_MuscleInfos.push_back( MuscleInfo( model, *mus ) );
				}
			}
		}

		MetaReflex::~MetaReflex()
		{
		}

		MetaReflex::MuscleInfo::MuscleInfo( sim::Model& model, sim::Muscle& mus ) : muscle( &mus ), moment_arm( REAL_NAN )
		{
			force_sensor = &model.AcquireDelayedSensor< sim::MuscleForceSensor >( mus );
			length_sensor = &model.AcquireDelayedSensor< sim::MuscleLengthSensor >( mus );
		}

		void MetaReflex::MuscleInfo::UpdateControls( MetaReflex& mr )
		{
			// length feedback
			Real ul = mr.length_gain * std::max( 0.0, length_sensor->GetValue( mr.delay ) - reference_length );

			// constant excitation
			Real uc = std::max( 0.0, moment_arm * mr.constant_moment );

			// force feedback
			Real uf = moment_arm * mr.force_feedback * std::max( 0.0, force_sensor->GetValue( mr.delay ) );

			muscle->AddControlValue( ul + uc + uf );
		}

		void MetaReflex::ComputeControls()
		{
			BOOST_FOREACH( MuscleInfo& mi, m_MuscleInfos )
				mi.UpdateControls( *this );
		}

		void MetaReflex::SetupUsingCurrentPose()
		{
			BOOST_FOREACH( MuscleInfo& mi, m_MuscleInfos )
			{
				mi.reference_length = mi.muscle->GetLength();
				mi.moment_arm = mi.muscle->GetMomentArm( target_dof );
			}
		}

		bool MetaReflex::MuscleCrossesDof( const sim::Muscle& mus )
		{
			// see if the muscle passes a joint that contains our dof
			const sim::Link& orgLink = mus.GetOriginLink();
			const sim::Link& insLink = mus.GetInsertionLink();
			const sim::Link* l = &insLink;
			while ( l && l != &orgLink )
			{
				if ( l->GetJoint().HasDof( target_dof.GetName() ) )
					return true;
				l = &l->GetParent();
			}
			return false;
		}

	}
}

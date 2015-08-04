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
#include "tools.h"

//#define DEBUG_MUSCLE "hamstrings_r"

namespace scone
{
	namespace cs
	{
		MetaReflex::MetaReflex( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area ) :
		target_dof( *FindByName( model.GetDofs(), props.GetStr( "target" ) + GetSideName( area.side ) ) )
		{
			// TODO: remove once a proper factory is used
			SCONE_ASSERT( props.GetStr( "type" ) == "MetaReflex" );

			opt::ScopedParamSetPrefixer prefixer( par, props.GetStr( "target" ) + "." );

			// TODO: fix hacky Degree / Rad conversion!
			INIT_PARAM_NAMED( props, par, reference_pos_in_degrees, "reference_pos", 0.0 );
			reference_pos_in_radians = DegToRad( reference_pos_in_degrees );

			INIT_PARAM( props, par, length_gain, 0.0 );
			INIT_PARAM( props, par, constant, 0.0 );
			INIT_PARAM( props, par, force_feedback, 0.0 );
			INIT_PROPERTY_REQUIRED( props, delay );

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
			log::Trace( target_dof.GetName() + ": reflexes=" + ToString( m_MuscleInfos.size() ) );
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
			Real uc = std::max( 0.0, moment_arm * mr.constant );

			// force feedback
			Real uf = moment_arm * mr.force_feedback * std::max( 0.0, force_sensor->GetValue( mr.delay ) );

			muscle->AddControlValue( ul + uc + uf );

#ifdef DEBUG_MUSCLE
			if ( muscle->GetName() == DEBUG_MUSCLE )
				log::TraceF( "length=%.3f reference=%.3f uf=%.3f", length_sensor->GetValue( mr.delay ), reference_length, ul );
#endif
		}

		void MetaReflex::UpdateControls()
		{
			BOOST_FOREACH( MuscleInfo& mi, m_MuscleInfos )
				mi.UpdateControls( *this );
		}

		void MetaReflex::SetupUsingCurrentPose()
		{
			BOOST_FOREACH( MuscleInfo& mi, m_MuscleInfos )
			{
				//mi.reference_length = ( mi.muscle->GetLength() - mi.muscle->GetTendonSlackLength() ) / mi.muscle->GetOptimalFiberLength();
				mi.reference_length = ( mi.muscle->GetLength() - mi.muscle->GetTendonSlackLength() ) / mi.muscle->GetOptimalFiberLength();
				mi.moment_arm = mi.muscle->GetMomentArm( target_dof );
				log::TraceF( "%-20s%-20sdof=% 8.3f length=% 8.3f moment=% 8.3f",
					target_dof.GetName().c_str(), mi.muscle->GetName().c_str(),
					target_dof.GetPos(), mi.reference_length, mi.moment_arm );
			}
		}

		bool MetaReflex::MuscleCrossesDof( const sim::Muscle& mus )
		{
			// see if the muscle passes a joint that contains our dof
			const sim::Link& orgLink = mus.GetOriginLink();
			const sim::Link& insLink = mus.GetInsertionLink();

			// ignore bi-articulair muscles!
			//if ( &insLink.GetParent() != &orgLink )
			//{
			//	log::Trace( "Ignoring muscle " + mus.GetName() + " origin=" + orgLink.GetBody().GetName()
			//		+ " insertion=" + insLink.GetBody().GetName()
			//		+ " parent=" + insLink.GetParent().GetBody().GetName() );
			//	return false;
			//}

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

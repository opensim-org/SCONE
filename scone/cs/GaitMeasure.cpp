#include "stdafx.h"
#include "GaitMeasure.h"
#include "../sim/Model.h"
#include "../sim/Body.h"

#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include "../core/Log.h"

namespace scone
{
	namespace cs
	{
		GaitMeasure::GaitMeasure( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area ) :
		Measure( props, par, model, area ),
		m_ActiveLegIndex( 0 )
		{
			INIT_FROM_PROP( props, termination_height, 0.5 );

			// get string of gait bodies
			String gait_bodies;
			INIT_FROM_PROP( props, gait_bodies, String() );

			// extract individual body names from gait_bodies string
			boost::char_separator< char > separator(", ");
			boost::tokenizer< boost::char_separator< char > > tokens( gait_bodies, separator );
			BOOST_FOREACH( const String& t, tokens )
			{
				size_t idx = model.FindBodyIndex( t );
				if ( idx != INVALID_INDEX )
					m_GaitBodies.push_back( &model.GetBody( idx ) );
			}

			m_InitialGaitDist = m_BestGaitDist = GetGaitDist( model, true );
			m_InitialComPos = model.GetComPos();
		}

		GaitMeasure::~GaitMeasure()
		{
		}

		void GaitMeasure::UpdateControls( sim::Model& model, double timestamp )
		{
			// check if this is a new step
			if ( model.GetIntegrationStep() == model.GetPreviousIntegrationStep() )
				return;

			// update best score
			m_BestGaitDist = std::max( m_BestGaitDist, GetGaitDist( model, false ) );

			// check if com is too low
			Vec3 com = model.GetComPos();
			if ( com.y < termination_height * m_InitialComPos.y )
				SetTerminationRequest();
		}

		double GaitMeasure::GetResult( sim::Model& model )
		{
			return 100 * ( m_BestGaitDist - m_InitialGaitDist );
		}

		scone::String GaitMeasure::GetSignature()
		{
			return "Gait";
		}

		Real GaitMeasure::GetGaitDist( sim::Model& model, bool init )
		{
			static const Real contact_threshold = 0.1;

			//bool contact = model.GetLeg( m_ActiveLegIndex ).GetContactForce().y > contact_threshold;
			//if ( init )
			//{
			//	m_ActiveLegContact = contact;
			//	m_ActiveLegInitDist = model.GetLeg( m_ActiveLegIndex ).GetFootLink().GetBody().GetPos().x;
			//	m_TotalDist = 0.0;
			//}

			//// compute distance, taking into account goals
			//static const Real stride_max = 1.0;
			//double leg_dist = std::min( model.GetLeg( m_ActiveLegIndex ).GetFootLink().GetBody().GetPos().x - m_ActiveLegInitDist, stride_max );
			//double com_dist = std::min( model.GetComPos().x - m_ActiveLegInitDist, stride_max * 0.75 );

			//if ( contact && !m_ActiveLegContact )
			//{
			//	// end of step
			//	m_TotalDist += ( leg_dist + com_dist ) / 2;

			//	// init new step
			//	m_ActiveLegIndex ^= 1;
			//	m_ActiveLegContact = model.GetLeg( m_ActiveLegIndex ).GetContactForce().y > contact_threshold;
			//	m_ActiveLegInitDist = model.GetLeg( m_ActiveLegIndex ).GetFootLink().GetBody().GetPos().x;

			//	return m_TotalDist;
			//}
			//else
			//{
			//	m_ActiveLegContact = contact;
			//	return m_TotalDist + ( leg_dist + com_dist ) / 2;
			//}

			// compute average of feet and Com (smallest 2 values)
			std::set< double > distances;
			double dist = REAL_MAX;
			BOOST_FOREACH( sim::LegUP& leg, model.GetLegs() )
				distances.insert( leg->GetFootLink().GetBody().GetPos().x );
			distances.insert( model.GetComPos().x );

			SCONE_ASSERT( distances.size() >= 2 );
			auto iter = distances.begin();
			return ( *iter + *(++iter) ) / 2;
		}
	}
}

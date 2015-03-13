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
		GaitMeasure::GaitMeasure( const PropNode& props, opt::ParamSet& par, sim::Model& model ) :
		Measure( props, par, model )
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

			// TODO: set initial gate dist to be the average of feet positions
			m_InitialGaitDist = GetGaitDist( model, true );
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

			// check if com is too low
			Vec3 com = model.GetComPos();
			if ( com.y < termination_height * m_InitialComPos.y )
				SetTerminationRequest();
		}

		double GaitMeasure::GetResult( sim::Model& model )
		{
			return 100 * ( GetGaitDist( model, false ) - m_InitialGaitDist );
		}

		scone::String GaitMeasure::GetSignature()
		{
			return "Gait";
		}

		Real GaitMeasure::GetGaitDist( sim::Model& model, bool init )
		{
			// get average position of legs that are in contact with the ground
			Real dist = 0.0;
			int nlegs = 0;
			BOOST_FOREACH( sim::LegUP& leg, model.GetLegs() )
			{
				if ( leg->GetContactForce().y > 0 || init )
				{
					dist += leg->GetFootLink().GetBody().GetPos().x;
					++nlegs;
				}
			}

			if ( nlegs > 0 )
				return dist / nlegs;
			else return 0.0;

			//if ( m_GaitBodies.empty() )
			//	return model.GetComPos().x;

			//// compute average pos of bodies
			//double dist = REAL_MAX;
			//BOOST_FOREACH( sim::Body* body, m_GaitBodies )
			//	dist = std::min( body->GetPos().x, dist );

			return dist;
		}
	}
}

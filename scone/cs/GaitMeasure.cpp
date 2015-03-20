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
		Measure( props, par, model, area )
		{
			INIT_FROM_PROP( props, termination_height, 0.5 );
			INIT_FROM_PROP( props, min_velocity, 0.5 );
			INIT_FROM_PROP( props, duration, 3.0 );

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

			m_InitBackDist = GetBackDist( model );
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

			// find normalized min velocity
			Real vel = GetRestrained( model.GetComVel().x, 0.0, min_velocity ) / min_velocity;
			m_MinVelocityMeasure.AddSample( vel, timestamp );

			// check termination
			bool terminate = timestamp >= duration;

			// check if com is too low
			Vec3 com = model.GetComPos();
			terminate |= com.y < termination_height * m_InitialComPos.y;
			if ( terminate )
			{
				if ( timestamp < duration )
				{
					// add penalty to min_velocity measure
					m_MinVelocityMeasure.AddSample( 0, timestamp );
					m_MinVelocityMeasure.AddSample( 0, duration );
				}
				SetTerminationRequest();
			}
		}

		double GaitMeasure::GetResult( sim::Model& model )
		{
			// find efficiency
			double dist = GetBackDist( model ) - m_InitBackDist;
			double cost = model.GetTotalEnergyConsumption() / 1000;
			double eff = dist / cost;

			double score = 100 * m_MinVelocityMeasure.GetAverage() * eff;
			//printf("dist=%f cost=%f eff=%f min_vel=%f score=%f\n", dist, cost, eff, m_MinVelocityMeasure.GetAverage(), score );

			return score;
		}

		scone::String GaitMeasure::GetSignature()
		{
			return "Gait";
		}

		scone::Real GaitMeasure::GetBackDist( sim::Model &model )
		{
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

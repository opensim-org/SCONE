#include "stdafx.h"
#include "GaitMeasure.h"
#include "../sim/Model.h"
#include "../sim/Body.h"

#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include "../core/Log.h"
#include "../sim/sim.h"
#include "../sim/Muscle.h"
#include "boost/format.hpp"

namespace scone
{
	namespace cs
	{
		GaitMeasure::GaitMeasure( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area ) :
		Measure( props, par, model, area ),
		m_EffortMeasure( props.GetChild( "EffortMeasure" ), par, model, area ),
		m_DofLimitMeasure( props.GetChild( "DofLimitMeasure" ), par, model, area ),
		m_MinVelocityMeasure( Statistic<>::NoInterpolation )
		{
			INIT_FROM_PROP( props, termination_height, 0.5 );
			INIT_FROM_PROP( props, min_velocity, 0.5 );
			INIT_FROM_PROP( props, contact_force_threshold, 0.1 );

			// init term weights
			const PropNode& weights = props.GetChild( "Weights" );
			for ( auto it = weights.Begin(); it != weights.End(); ++it )
				m_Terms[ it->first ] = it->second->GetValue< Real >();

			// get string of gait bodies
			String gait_bodies;
			INIT_FROM_PROP( props, gait_bodies, String() );

			// extract individual body names from gait_bodies string
			boost::char_separator< char > separator(", ");
			boost::tokenizer< boost::char_separator< char > > tokens( gait_bodies, separator );
			BOOST_FOREACH( const String& t, tokens )
			{
				size_t idx = model.FindBodyIndex( t );
				if ( idx != NoIndex )
					m_GaitBodies.push_back( &model.GetBody( idx ) );
			}

			m_InitGaitDist = m_PrevGaitDist = GetGaitDist( model );
			m_InitialComPos = model.GetComPos();
			model.GetComVel();
		}

		GaitMeasure::~GaitMeasure()
		{
		}

		void GaitMeasure::UpdateControls( sim::Model& model, double timestamp )
		{
			// check if this is a new step
			if ( model.GetIntegrationStep() == model.GetPreviousIntegrationStep() )
				return;

			// update energy measure
			m_EffortMeasure.UpdateControls( model, timestamp );
			m_DofLimitMeasure.UpdateControls( model, timestamp );

			// check termination
			bool terminate = false;
			terminate |= model.GetComPos().y < termination_height * m_InitialComPos.y; // COM too low

			// update min_velocity measure on new step or termination
			if ( HasNewFootContact( model ) || terminate )
			{
				double gait_dist = GetGaitDist( model );
				double step_size = gait_dist - m_PrevGaitDist;
				double dt = model.GetTime() - m_MinVelocityMeasure.GetPrevTime();
				if ( dt > 0 )
				{
					double norm_vel = GetRestrained( ( step_size / dt ) / min_velocity, 0.0, 1.0 );
					m_MinVelocityMeasure.AddSample( norm_vel, timestamp );
					log::TraceF( "%.3f: STEP! step_size=%.3f dt=%.3f norm_vel=%.3f", timestamp, step_size, dt, norm_vel );
				}
				m_PrevGaitDist = gait_dist;
			}

			// handle termination
			if ( terminate )
			{
				log::TraceF( "%.3f: Terminating simulation", timestamp );
				SetTerminationRequest();
			}
		}

		double GaitMeasure::GetResult( sim::Model& model )
		{
			// precompute some values
			double distance = GetGaitDist( model ) - m_InitGaitDist;
			double speed = distance / model.GetTime();
			double duration = model.GetSimulationEndTime();

			// add penalty to min_velocity measure
			if ( model.GetTime() < duration )
				m_MinVelocityMeasure.AddSample( 0, duration );

			// find efficiency
			m_Terms[ "effort" ].value = m_EffortMeasure.GetResult( model ) / model.GetMass();
			m_Terms[ "distance" ].value = 1.0 - std::min( 1.0, distance / ( min_velocity * duration ) );
			m_Terms[ "velocity" ].value = 1.0 - m_MinVelocityMeasure.GetAverage();
			m_Terms[ "balance" ].value = 1.0 - ( model.GetTime() / std::max( duration, model.GetTime() ) );

			// for cost_of_transport, we use speed because effort is an average
			// speed is capped to 0.01 prevent high or negative values for cost_of_transport
			m_Terms[ "cost_of_transport" ].value = m_Terms[ "effort" ].value / std::max( 0.01, speed );
			m_Terms[ "limit" ].value = m_DofLimitMeasure.GetResult( model );

			// generate report and count total score
			// TODO: use generic measure report?
			double score = 0.0;
			BOOST_FOREACH( StringWeightedTermPair& term, m_Terms )
			{
				log::DebugF( "%20s\t%8.3f\t%8.3f\t%8.3f", term.first.c_str(), term.second.weighted_value(), term.second.value, term.second.weight );
				score += term.second.weighted_value();
			}

			log::DebugF( "%20s\t%8.3f", "TOTAL", score );
			return score;
		}

		String GaitMeasure::GetSignature()
		{
			String s = GetStringF( "V%02d", static_cast< int >( 10 * min_velocity ) );

			String extra;
			BOOST_FOREACH( StringWeightedTermPair& term, m_Terms )
			{
				// add the first character of each of the used weight terms
				if ( term.second.weight > 0.0 )
					extra += term.first[0];
			}

			if ( extra != "clv" )
				s += extra; // add only if it's not the default

			return s;
		}

		scone::Real GaitMeasure::GetGaitDist( sim::Model &model )
		{
			// compute average of feet and Com (smallest 2 values)
			std::set< double > distances;
			BOOST_FOREACH( sim::LegUP& leg, model.GetLegs() )
				distances.insert( leg->GetFootLink().GetBody().GetPos().x );
			distances.insert( model.GetComPos().x );

			SCONE_ASSERT( distances.size() >= 2 );
			auto iter = distances.begin();
			return ( *iter + *(++iter) ) / 2;
		}

		bool GaitMeasure::HasNewFootContact( sim::Model& model )
		{
			if ( m_PrevContactState.empty() )
			{
				// initialize
				BOOST_FOREACH( sim::LegUP& leg, model.GetLegs() )
					m_PrevContactState.push_back( leg->GetContactForce().y > contact_force_threshold );
				return false;
			}

			bool has_new_contact = false;
			for ( size_t idx = 0; idx < model.GetLegCount(); ++idx )
			{
				bool contact = model.GetLeg( idx ).GetContactForce().y > contact_force_threshold;
				has_new_contact |= contact && !m_PrevContactState[ idx ];
				m_PrevContactState[ idx ] = contact;
			}

			return has_new_contact;
		}
	}
}

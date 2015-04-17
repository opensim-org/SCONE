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
		m_DofLimitMeasure( props.GetChild( "DofLimitMeasure" ), par, model, area )
		{
			INIT_FROM_PROP( props, termination_height, 0.5 );
			INIT_FROM_PROP( props, min_velocity, 0.5 );
			INIT_FROM_PROP( props, duration, 3.0 );

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
			m_Terms[ "effort" ].value = m_EffortMeasure.GetResult( model ) / model.GetMass();
			m_Terms[ "distance" ].value = GetGaitDist( model ) - m_InitGaitDist;
			m_Terms[ "speed" ].value = m_Terms[ "distance" ].value / model.GetTime();
			m_Terms[ "min_velocity" ].value = std::max( 0.0, 1.0 - ( m_Terms[ "distance" ].value / duration ) / min_velocity );

			// for cost_of_transport, we use speed because effort is an average
			// speed is capped to min_velocity to prevent high values for cost_of_transport
			m_Terms[ "cost_of_transport" ].value = m_Terms[ "effort" ].value / std::max( min_velocity, m_Terms[ "speed" ].value );
			m_Terms[ "dof_limit" ].value = m_DofLimitMeasure.GetResult( model );

			// generate report and count total score
			// TODO: use generic measure report?
			double score = 0.0;
			BOOST_FOREACH( StringWeightedTermPair& term, m_Terms )
			{
				log::DebugF( "%20s\t%6.2f\t%6.2f\t%6.2f", term.first.c_str(), term.second.weighted_value(), term.second.value, term.second.weight );
				score += term.second.weighted_value();
			}

			log::DebugF( "%20s\t%6.2f", "TOTAL", score );
			return score;
		}

		scone::String GaitMeasure::GetSignature()
		{
			return "gm";
		}

		scone::Real GaitMeasure::GetGaitDist( sim::Model &model )
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

		double GaitMeasure::GetModelEnergy( sim::Model& model )
		{
			double f = 1.0; // base muscle force
			BOOST_FOREACH( sim::MuscleUP& mus, model.GetMuscles() )
			{
				f += mus->GetForce();
			}

			return f;
		}

	}
}

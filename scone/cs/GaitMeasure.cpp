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
		const int g_GaitMeasureVersion = 2;

		GaitMeasure::GaitMeasure( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area ) :
		Measure( props, par, model, area ),
		m_EffortMeasure( props.GetChild( "EffortMeasure" ), par, model, area )
		{
			INIT_FROM_PROP( props, termination_height, 0.5 );
			INIT_FROM_PROP( props, min_velocity, 0.5 );
			INIT_FROM_PROP( props, duration, 3.0 );

			INIT_FROM_PROP( props, min_velocity_weight, 100.0 );
			INIT_FROM_PROP( props, efficiency_weight, 0.001 );

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

			m_InitBackDist = GetBackDist( model );
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

			// find normalized min velocity
			Real vel = GetRestrained( model.GetComVel().x, 0.0, min_velocity ) / min_velocity;
			m_MinVelocityMeasure.AddSample( vel, timestamp );

			// update energy measure
			m_EffortMeasure.UpdateControls( model, timestamp );

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
			double effort = m_EffortMeasure.GetResult( model );
			double distance = GetBackDist( model ) - m_InitBackDist;
			double efficiency = effort / std::max( 0.1, distance );
			double min_vel_score = 1.0 - m_MinVelocityMeasure.GetAverage();

			double score = min_velocity_weight * min_vel_score + efficiency_weight * efficiency;

			// update report
			m_Report.Set( "distance", distance );
			m_Report.Set( "effort", effort );
			//m_Report.Set( "OpenSimEffort", model.GetTotalEnergyConsumption() );
			//m_Report.Set( "OpenSimEffortFact", model.GetTotalEnergyConsumption() / cost );
			m_Report.Set( "weighted_efficiency", efficiency_weight * efficiency );
			m_Report.Set( "weighted_min_velocity", min_velocity_weight * min_vel_score );
			m_Report.Set( score );

			if ( _isnan( score ) )
				printf("dist=%f cost=%f eff=%f min_vel=%f score=%f\n", distance, effort, efficiency, m_MinVelocityMeasure.GetAverage(), score );

			return score;
		}

		scone::String GaitMeasure::GetSignature()
		{
			return GetStringF( "GM%d", g_GaitMeasureVersion );
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

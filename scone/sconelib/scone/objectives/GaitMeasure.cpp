#include <set>

#include "GaitMeasure.h"
#include "scone/model/Model.h"
#include "scone/model/Body.h"
#include "scone/core/Log.h"
#include "scone/model/Muscle.h"
#include "scone/core/Profiler.h"
#include "scone/core/Range.h"

namespace scone
{
	GaitMeasure::GaitMeasure( const PropNode& props, Params& par, Model& model, const Locality& area ) :
		Measure( props, par, model, area ),
		m_MinVelocityMeasure( Statistic<>::NoInterpolation ),
		m_nSteps( 0 ),
		m_TotStepSize( 0.0 )
	{
		INIT_PROPERTY( props, termination_height, 0.5 );
		INIT_PROPERTY( props, min_velocity, 0.5 );
		INIT_PROPERTY( props, max_velocity, 299792458.0 ); // default max velocity = speed of light
		INIT_PROPERTY( props, load_threshold, 0.1 );

		// get string of gait bodies
		String gait_bodies;
		INIT_PROPERTY( props, gait_bodies, String() );

		// extract individual body names from gait_bodies string
		auto tokens = flut::split_str( gait_bodies, ", " );
		for ( const String& t : tokens )
		{
			Body& b = *FindByName( model.GetBodies(), t );
			m_GaitBodies.push_back( &b );
		}

		m_InitGaitDist = m_PrevGaitDist = GetGaitDist( model );
		m_InitialComPos = model.GetComPos();

		// add an initial sample to m_MinVelocityMeasure because the first sample is ignored with NoInterpolation
		m_MinVelocityMeasure.AddSample( 0.0, 0.0 );
	}

	GaitMeasure::~GaitMeasure()
	{
	}

	Controller::UpdateResult GaitMeasure::UpdateAnalysis( const Model& model, double timestamp )
	{
		SCONE_PROFILE_FUNCTION;

		// make sure this is a new step
		SCONE_ASSERT( model.GetIntegrationStep() != model.GetPreviousIntegrationStep() );
		if ( !IsActive( model, timestamp ) )
			return NoUpdate;

		// check termination
		bool terminate = false;
		terminate |= model.GetComPos().y < termination_height * m_InitialComPos.y; // COM too low

		// update min_velocity measure on new step
		if ( HasNewFootContact( model ) )
		{
			m_nSteps++;
			m_TotStepSize = model.GetComPos().x - m_InitialComPos.x;
			UpdateVelocityMeasure( model, timestamp );
		}

		// handle termination
		if ( terminate )
		{
			log::TraceF( "%.3f: Terminating simulation", timestamp );
			return RequestTermination;
		}
		else return SuccessfulUpdate;
	}

	double GaitMeasure::GetResult( Model& model )
	{
		// precompute some values
		double distance = GetGaitDist( model ) - m_InitGaitDist;
		double speed = distance / model.GetTime();
		double duration = model.GetSimulationEndTime();

		// add final step and penalty to min_velocity measure
		UpdateVelocityMeasure( model, model.GetTime() );
		if ( model.GetTime() < duration )
			m_MinVelocityMeasure.AddSample( duration, 0 );

		GetReport().set( "balance", 1.0 - ( model.GetTime() / std::max( duration, model.GetTime() ) ) );
		GetReport().set( "min_velocity", 1.0 - m_MinVelocityMeasure.GetAverage() );
		GetReport().set( "distance", distance );
		GetReport().set( "speed", speed );
		GetReport().set( "steps", m_nSteps );
		GetReport().set( "stepsize", m_TotStepSize / m_nSteps );

		return 1.0 - m_MinVelocityMeasure.GetAverage();
	}

	void GaitMeasure::UpdateVelocityMeasure( const Model &model, double timestamp )
	{
		double gait_dist = GetGaitDist( model );
		double step_size = gait_dist - m_PrevGaitDist;
		double dt = model.GetTime() - m_MinVelocityMeasure.GetPrevTime();
		if ( dt > 0 )
		{
			double step_vel = step_size / dt;
			double penalty = Range< double >( min_velocity, max_velocity ).GetRangeViolation( step_vel );
			double norm_vel = std::max( 0.0, 1.0 - ( fabs( penalty ) / min_velocity ) );

			m_MinVelocityMeasure.AddSample( timestamp, norm_vel );
			log::TraceF( "%.3f: step_velocity=%.3f (%.3f/%.3f) penalty=%.3f norm_vel=%.3f",
				timestamp, step_vel, step_size, dt, penalty, norm_vel );
		}
		m_PrevGaitDist = gait_dist;
	}

	scone::Real GaitMeasure::GetGaitDist( const Model &model )
	{
		// compute average of feet and Com (smallest 2 values)
		std::set< double > distances;
		for ( const LegUP& leg : model.GetLegs() )
			distances.insert( leg->GetFootLink().GetBody().GetComPos().x );
		distances.insert( model.GetComPos().x );

		SCONE_ASSERT( distances.size() >= 2 );
		auto iter = distances.begin();
		return ( *iter + *( ++iter ) ) / 2;
	}

	String GaitMeasure::GetClassSignature() const
	{
		return stringf( "S%02d", static_cast<int>( 10 * min_velocity ) );
	}

	bool GaitMeasure::HasNewFootContact( const Model& model )
	{
		if ( m_PrevContactState.empty() )
		{
			// initialize
			for ( const LegUP& leg : model.GetLegs() )
				m_PrevContactState.push_back( leg->GetLoad() >= load_threshold );
			return false;
		}

		bool has_new_contact = false;
		for ( size_t idx = 0; idx < model.GetLegCount(); ++idx )
		{
			bool contact = model.GetLeg( idx ).GetLoad() >= load_threshold;
			if ( contact && !m_PrevContactState[ idx ] )
			{
				has_new_contact = true;
				//log::TraceF( "%.3f: Step detected for leg %d", model.GetTime(), idx );
			}
			m_PrevContactState[ idx ] = contact;
		}

		return has_new_contact;
	}
}

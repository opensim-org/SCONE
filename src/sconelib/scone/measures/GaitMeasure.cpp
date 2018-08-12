#include "GaitMeasure.h"
#include "scone/model/Model.h"
#include "scone/model/Body.h"
#include "scone/core/Log.h"
#include "scone/model/Muscle.h"
#include "scone/core/Profiler.h"
#include "scone/core/Range.h"
#include "xo/container/sorted_vector.h"
#include "xo/container/container_algorithms.h"

namespace scone
{
	GaitMeasure::GaitMeasure( const PropNode& props, Params& par, Model& model, const Locality& area ) :
	Measure( props, par, model, area ),
	m_VelocityMeasure( Statistic<>::NoInterpolation )
	{
		INIT_PROPERTY( props, termination_height, 0.5 );
		INIT_PROPERTY( props, min_velocity, 0.5 );
		INIT_PROPERTY( props, max_velocity, 299792458.0 ); // default max velocity = speed of light
		INIT_PROPERTY( props, load_threshold, 0.1 );
		INIT_PROPERTY( props, min_step_duration, 0.1 );
		INIT_PROPERTY( props, initiation_step_count, 3 );

		// get string of gait bodies
		String gait_bodies;
		INIT_PROPERTY( props, gait_bodies, String() );

		// extract individual body names from gait_bodies string
		auto tokens = xo::split_str( gait_bodies, ", " );
		for ( const String& t : tokens )
		{
			Body& b = *FindByName( model.GetBodies(), t );
			m_GaitBodies.push_back( &b );
		}

		m_InitGaitDist = m_PrevGaitDist = GetGaitDist( model );
		m_InitialComPos = model.GetComPos();

		// add an initial sample to m_MinVelocityMeasure because the first sample is ignored with NoInterpolation
		m_VelocityMeasure.AddSample( 0.0, 0.0 );
	}

	GaitMeasure::~GaitMeasure()
	{
	}

	bool GaitMeasure::UpdateMeasure( const Model& model, double timestamp )
	{
		SCONE_PROFILE_FUNCTION;

		// make sure this is a new step
		SCONE_ASSERT( model.GetIntegrationStep() != model.GetPreviousIntegrationStep() );

		// check termination
		bool terminate = false;
		terminate |= model.GetComPos().y < termination_height * m_InitialComPos.y; // COM too low

		// update min_velocity measure on new step
		if ( HasNewFootContact( model ) )
			UpdateVelocityMeasure( model, timestamp );

		// handle termination
		if ( terminate )
		{
			log::TraceF( "%.3f: Terminating simulation", timestamp );
			return true;
		}
		else return false;
	}

	double GaitMeasure::GetResult( Model& model )
	{
		// precompute some values
		double distance = GetGaitDist( model ) - m_InitGaitDist;
		double speed = distance / model.GetTime();
		double duration = model.GetSimulationEndTime();
		size_t step_count = m_StepLengths.size();

		// add final step and penalty to min_velocity measure
		// TODO: get rid of this? it's not a full step
		UpdateVelocityMeasure( model, model.GetTime() );

		// compute measure based on step data
		double alt_measure = 0.0;
		int start_step = xo::clamped( int( step_count ) - initiation_step_count, 0, initiation_step_count );
		TimeInSeconds step_time = 0.0;
		for ( int step = start_step; step < step_count; ++step )
		{
			double step_vel = m_StepLengths[ step ] / m_StepDurations[ step ];
			double range_error = Range< double >( min_velocity, max_velocity ).GetRangeViolation( step_vel );
			double norm_vel = std::max( 0.0, 1.0 - ( fabs( range_error ) / min_velocity ) );

			log::TraceF( "%.3f: step=%d step_velocity=%.3f (%.3f/%.3f) range_error=%.3f norm_vel=%.3f",
				step_time, step, step_vel, m_StepLengths[ step ], m_StepDurations[ step ], range_error, norm_vel );

			alt_measure += m_StepDurations[ step ] * norm_vel;
			step_time += m_StepDurations[ step ];
		}

		// add penalty for falling early
		if ( model.GetTime() < duration )
		{
			m_VelocityMeasure.AddSample( duration, 0 );
			step_time += duration - model.GetTime();
		}

		GetReport().set( "speed", speed );
		GetReport().set( "step_count", step_count );
		GetReport().set( "step_length", xo::average( m_StepLengths ) );
		GetReport().set( "step_duration", xo::average( m_StepDurations ) );

#if 0
		GetReport().set( "alt_result", 1.0 - alt_measure / step_time );
		return 1.0 - m_VelocityMeasure.GetAverage();
#else
		GetReport().set( "org_result", 1.0 - m_VelocityMeasure.GetAverage() );
		return 1.0 - alt_measure / step_time;
#endif
	}

	void GaitMeasure::UpdateVelocityMeasure( const Model &model, double timestamp )
	{
		double gait_dist = GetGaitDist( model );
		double step_length = gait_dist - m_PrevGaitDist;
		double dt = model.GetTime() - m_VelocityMeasure.GetPrevTime();
		if ( dt > min_step_duration )
		{
			m_StepLengths.push_back( step_length );
			m_StepDurations.push_back( dt );

			double step_vel = step_length / dt;
			double penalty = Range< double >( min_velocity, max_velocity ).GetRangeViolation( step_vel );
			double norm_vel = std::max( 0.0, 1.0 - ( fabs( penalty ) / min_velocity ) );

			m_VelocityMeasure.AddSample( timestamp, norm_vel );
			log::TraceF( "%.3f: step_velocity=%.3f (%.3f/%.3f) penalty=%.3f norm_vel=%.3f",
				timestamp, step_vel, step_length, dt, penalty, norm_vel );
		}
		m_PrevGaitDist = gait_dist;
	}

	scone::Real GaitMeasure::GetGaitDist( const Model &model )
	{
		// compute average of feet and Com (smallest 2 values)
		// TODO: make body name configurable and find it in the constructor
		xo::sorted_vector< double > distances;
		for ( const LegUP& leg : model.GetLegs() )
			distances.insert( leg->GetFootLink().GetChild( 0 ).GetBody().GetComPos().x );
		distances.insert( model.GetComPos().x );

		SCONE_ASSERT( distances.size() >= 2 );
		return ( distances[ 0 ] + distances[ 1 ] ) / 2;
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

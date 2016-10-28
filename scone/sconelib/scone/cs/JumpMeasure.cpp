#include "JumpMeasure.h"
#include "scone/sim/Area.h"

namespace scone
{
	namespace cs
	{
		JumpMeasure::JumpMeasure( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area ) :
		Measure( props, par, model, area ),
		target_body( nullptr ),
		state( Prepare ),
		init_min_x( 10000.0 )
		{
			INIT_PROPERTY( props, termination_height, 0.5 );
			INIT_PROPERTY( props, prepare_time, 0.2 );
			INIT_PROPERTY( props, terminate_on_peak, true );
			INIT_PROPERTY( props, negate_result, false );
			INIT_PROPERTY( props, jump_type, int( HighJump ) );

			if ( props.HasKey( "target_body" ) )
				target_body = FindByName( model.GetBodies(), props.GetStr( "target_body" ) ).get();

			prepare_com = init_com = model.GetComPos();

			for ( auto& body : model.GetBodies() )
				init_min_x = std::min( init_min_x, body->GetComPos().x );
		}

		JumpMeasure::~JumpMeasure() { }

		double JumpMeasure::GetResult( sim::Model& model )
		{
			switch ( jump_type )
			{
			case scone::cs::JumpMeasure::HighJump:
				return GetHighJumpResult( model );
				break;
			case scone::cs::JumpMeasure::LongJump:
				return GetLongJumpResult( model );
				break;
			default:
				SCONE_THROW( "Invalid jump type" );
				break;
			}
		}

		sim::Controller::UpdateResult JumpMeasure::UpdateAnalysis( const sim::Model& model, double timestamp )
		{
			if ( !IsActive( model, timestamp ) )
				return NoUpdate;

			Vec3 pos = target_body ? target_body->GetComPos() : model.GetComPos();
			Vec3 vel = target_body ? target_body->GetLinVel() : model.GetComVel();

			switch ( state )
			{
			case Prepare:
				if ( pos.y < termination_height * init_com.y )
					return RequestTermination;

				if ( timestamp >= prepare_time && vel.y > 0 )
				{
					state = Takeoff;
					prepare_com = pos;
				}
				break;

			case Takeoff:
				if ( model.GetTotalContactForce() <= 0 )
					state = Flight;
				if ( terminate_on_peak && vel.y < 0 )
					return RequestTermination;
				break;

			case Flight:
				if ( vel.y < 0 )
				{
					if ( terminate_on_peak )
						return RequestTermination;
					else state = Landing;
				}
				if ( model.GetTotalContactForce() > 0 )
				{
					if ( terminate_on_peak )
						return RequestTermination;
					else state = Recover;
				}

			case Landing:
				if ( model.GetTotalContactForce() > 0 )
					state = Recover;
				break;

			case Recover:
				break;
			}

			return Controller::SuccessfulUpdate;
		}

		scone::String JumpMeasure::GetClassSignature() const 
		{
			switch ( jump_type )
			{
			case scone::cs::JumpMeasure::HighJump: return "HighJump";
			case scone::cs::JumpMeasure::LongJump: return "LongJump";
			default:
				SCONE_THROW( "Invalid jump type" );
				break;
			}
		}

		double JumpMeasure::GetHighJumpResult( const sim::Model& model )
		{
			Vec3 pos = target_body ? target_body->GetComPos() : model.GetComPos();

			double early_jump_penalty = 100 * std::max( 0.0, prepare_com.y - init_com.y );
			double jump_height = 100 * pos.y;
			double result = 0.0;

			switch ( state )
			{
			case scone::cs::JumpMeasure::Prepare:
				// failed during preparation, return projected height after 1s
				result = 100 * ( init_com.y + ( pos.y - init_com.y ) / model.GetTime() );
				break;
			case scone::cs::JumpMeasure::Takeoff:
			case scone::cs::JumpMeasure::Flight:
			case scone::cs::JumpMeasure::Landing:
				// we've managed to jump, return height as result, with penalty for early jumping
				result = jump_height - early_jump_penalty;
				break;
			}

			if ( negate_result )
				result = -result;

			GetReport().Set( result );
			GetReport().Set( "jump_height", jump_height );
			GetReport().Set( "early_jump_penalty", early_jump_penalty );

			return result;
		}

		double JumpMeasure::GetLongJumpResult( const sim::Model& model )
		{
			Vec3 pos = target_body ? target_body->GetComPos() : model.GetComPos();
			Vec3 vel = target_body ? target_body->GetLinVel() : model.GetComVel();

			// get position of leftmost body
			double min_pos_x = 1000.0;
			double min_pos_y = 1000.0;
			for ( auto& body : model.GetBodies() )
			{
				min_pos_x = std::min( min_pos_x, body->GetComPos().x );
				min_pos_y = std::min( min_pos_y, body->GetComPos().y );
			}

			double early_jump_penalty = 100 * std::max( 0.0, prepare_com.y - init_com.y );
			double jump_height = 100 * pos.y;
			double jump_dist = 100 * ( min_pos_x - init_min_x );

			// compute results based on state
			double com_landing_distance = 100 * GetComLandingDist( model );
			double result = com_landing_distance - early_jump_penalty;
			log::info( "result=", result );

#if 0
			switch ( state )
			{
			case scone::cs::JumpMeasure::Prepare:
				// failed during preparation, return projected height after 1s
				result = 100 * ( init_com.y + ( pos.y - init_com.y ) / model.GetTime() );
				break;
			case scone::cs::JumpMeasure::Takeoff:
				// we've managed to take-off, return height as result, with penalty for early jumping
				result = jump_height - early_jump_penalty;
				break;
			case scone::cs::JumpMeasure::Flight:
			case scone::cs::JumpMeasure::Landing:
				// we've managed to take-off, return height as result, with penalty for early jumping
				result = jump_height + jump_dist - early_jump_penalty;
			}
#endif
			if ( negate_result ) result = -result;

			GetReport().Set( result );
			GetReport().Set( "jump_height", jump_height );
			GetReport().Set( "jump_distance", jump_dist );
			GetReport().Set( "com_landing_distance", com_landing_distance );
			GetReport().Set( "early_jump_penalty", early_jump_penalty );

			return result;

		}

		double JumpMeasure::GetComLandingDist( const sim::Model& m )
		{
			Vec3 com_pos = m.GetComPos();
			Vec3 com_vel = m.GetComVel();

			double g = -9.81;
			double y0 = com_pos.y;
			double dy = com_vel.y;
			double t = ( -dy - sqrt( dy * dy - 2 * g * y0 ) ) / g;
			double pos = com_pos.x + t * com_vel.x;

			GetReport().Set( "com_landing_time", t );
			GetReport().Set( "com_landing_y0", com_pos.y );
			GetReport().Set( "com_landing_dy", com_vel.y );
			GetReport().Set( "com_landing_x0", com_pos.x );
			GetReport().Set( "com_landing_dx", com_vel.x );

			return pos;
		}
	}
}

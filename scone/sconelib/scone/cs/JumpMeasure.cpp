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

			Vec3 com_pos = model.GetComPos();
			Vec3 com_vel = model.GetComVel();

			if ( com_pos.y < termination_height * init_com.y )
			{
				log::trace( timestamp, ": Terminating, com_pos=", com_pos );
				return RequestTermination;
			}

			switch ( state )
			{
			case Prepare:
				if ( timestamp >= prepare_time && com_vel.y > 0 )
				{
					state = Takeoff;
					prepare_com = com_pos;
					log::trace( timestamp, ": State changed to Takeoff, prepare_com=", prepare_com );
				}
				break;

			case Takeoff:
				if ( model.GetTotalContactForce() <= 0 )
				{
					state = Flight;
					log::trace( timestamp, ": State changed to Flight, com_pos=", com_pos );
				}
				if ( terminate_on_peak && com_vel.y < 0 )
					return RequestTermination;
				break;

			case Flight:
				if ( com_vel.y < 0 )
				{
					state = Landing;
					log::trace( timestamp, ": State changed to Landing, com_pos=", com_pos );
					if ( terminate_on_peak )
						return RequestTermination;
				}
				if ( model.GetTotalContactForce() > 0 )
				{
					state = Recover;
					log::trace( timestamp, ": State changed to Recover, com_pos=", com_pos );
					if ( terminate_on_peak )
						return RequestTermination;
				}

			case Landing:
				if ( model.GetTotalContactForce() > 0 )
				{
					state = Recover;
					log::trace( timestamp, ": State changed to Recover, com_pos=", com_pos );
				}
				break;

			case Recover:
				return RequestTermination;
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
			Vec3 com_pos = model.GetComPos();
			Vec3 com_vel = model.GetComVel();

			double early_jump_penalty = 100 * std::max( 0.0, prepare_com.y - init_com.y );
			double com_landing_distance = 100 * GetLandingDist( com_pos, com_vel );
			double body_landing_distance = target_body ? 100 * GetLandingDist( target_body->GetComPos(), target_body->GetLinVel() ) : 1000.0;

			double result = std::min( com_landing_distance, body_landing_distance ) - early_jump_penalty;

			if ( negate_result ) result = -result;

			GetReport().Set( result );
			GetReport().Set( "com_landing_distance", com_landing_distance );
			GetReport().Set( "body_landing_distance", body_landing_distance );
			GetReport().Set( "early_jump_penalty", early_jump_penalty );

			return result;
		}

		double JumpMeasure::GetLandingDist( const Vec3& pos, const Vec3& vel )
		{
			double t = 0.0;
			double g = -9.81;
			double disc = vel.y * vel.y - 2 * g * pos.y;
			if ( disc > 0 )
				t = ( -vel.y - sqrt( vel.y * vel.y - 2 * g * pos.y ) ) / g; // polynomial has two roots
			else
				t = ( -vel.y - sqrt( vel.y * vel.y + 2 * g * pos.y ) ) / g; // polynomial has one or complex root

			return pos.x + t * vel.x;
		}
	}
}

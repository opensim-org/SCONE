#include "JumpMeasure.h"
#include "scone/model/Locality.h"

namespace scone
{
	JumpMeasure::JumpMeasure( const PropNode& props, Params& par, Model& model, const Locality& area ) :
		Measure( props, par, model, area ),
		target_body( nullptr ),
		state( Prepare ),
		init_min_x( 10000.0 ),
		recover_start_time( 0 )
	{
		INIT_PROPERTY( props, termination_height, 0.5 );
		INIT_PROPERTY( props, prepare_time, 0.2 );
		INIT_PROPERTY( props, recover_time, 1.0 );
		INIT_PROPERTY( props, terminate_on_peak, true );
		INIT_PROPERTY( props, negate_result, false );
		INIT_PROPERTY( props, jump_type, int( HighJump ) );
		INIT_PROPERTY( props, minimize, false ); // defaults to false

		if ( auto body = props.try_get< String >( "body" ) )
			target_body = FindByName( model.GetBodies(), *body ).get();

		prepare_com = init_com = GetTargetPos( model );
		peak_height = init_com.y;

		for ( auto& body : model.GetBodies() )
			init_min_x = std::min( init_min_x, body->GetComPos().x );
	}

	JumpMeasure::~JumpMeasure() { }

	scone::Vec3 JumpMeasure::GetTargetPos( const Model& m ) const
	{
		return target_body ? target_body->GetComPos() : m.GetComPos();
	}

	double JumpMeasure::GetResult( Model& model )
	{
		switch ( jump_type )
		{
		case scone::JumpMeasure::HighJump:
			return GetHighJumpResult( model );
			break;
		case scone::JumpMeasure::LongJump:
			return GetLongJumpResult( model );
			break;
		default:
			SCONE_THROW( "Invalid jump type" );
			break;
		}
	}

	bool JumpMeasure::UpdateMeasure( const Model& model, double timestamp )
	{
		Vec3 com_pos = model.GetComPos();
		Vec3 com_vel = model.GetComVel();
		double grf = model.GetTotalContactForce();

		if ( com_pos.y < termination_height * init_com.y )
		{
			log::trace( timestamp, ": Terminating, com_pos=", com_pos );
			return true;
		}

		Vec3 pos = GetTargetPos( model );
		peak_height = xo::max( peak_height, pos.y );

		switch ( state )
		{
		case Prepare:
			if ( timestamp >= prepare_time && ( com_vel.y > 0 || grf <= 0 ) )
			{
				state = Takeoff;
				prepare_com = com_pos;
				log::trace( timestamp, ": State changed to Takeoff, prepare_com=", prepare_com );
			}
			break;

		case Takeoff:
			if ( grf <= 0 )
			{
				state = Flight;
				log::trace( timestamp, ": State changed to Flight, com_pos=", com_pos );
			}
			else if ( com_vel.y < 0 )
			{
				// unsuccessful take-off
				state = Recover;
				peak_com = com_pos;
				peak_com_vel = com_vel;
				recover_com = com_pos;
				log::trace( timestamp, ": State changed to Recover, com_pos=", com_pos );

				if ( terminate_on_peak )
					return true;
			}
			break;

		case Flight:
			if ( com_vel.y < 0 || grf > 0 )
			{
				state = Landing;
				peak_com = com_pos;
				peak_com_vel = com_vel;
				log::trace( timestamp, ": State changed to Landing, com_pos=", com_pos );

				if ( terminate_on_peak )
					return true;
			}
			break;

		case Landing:
			if ( grf > 0 )
			{
				state = Recover;
				recover_com = com_pos;
				recover_start_time = timestamp;
				log::trace( timestamp, ": State changed to Recover, com_pos=", com_pos );
			}
			break;

		case Recover:
			log::trace( "com_vel=", com_vel.x );
			recover_cop_dist = std::min( recover_cop_dist, model.GetLeg( 0 ).GetContactCop().x );

			if ( timestamp - recover_start_time >= recover_time )
				return true;
			break;
		}

		return false;
	}

	scone::String JumpMeasure::GetClassSignature() const
	{
		switch ( jump_type )
		{
		case scone::JumpMeasure::HighJump: return "HighJump";
		case scone::JumpMeasure::LongJump: return "LongJump";
		default:
			SCONE_THROW( "Invalid jump type" );
			break;
		}
	}

	double JumpMeasure::GetHighJumpResult( const Model& model )
	{
		Vec3 pos = GetTargetPos( model );
		peak_height = xo::max( peak_height, pos.y );

		double early_jump_penalty = 100 * std::max( 0.0, prepare_com.y - init_com.y );
		double jump_height = 100 * peak_height;
		double result = 0.0;

		switch ( state )
		{
		case scone::JumpMeasure::Prepare:
			// failed during preparation, return projected height after 1s
			result = 100 * ( init_com.y + ( pos.y - init_com.y ) / model.GetTime() );
			break;
		case scone::JumpMeasure::Takeoff:
		case scone::JumpMeasure::Flight:
		case scone::JumpMeasure::Landing:
		case scone::JumpMeasure::Recover:
			// we've managed to jump, return height as result, with penalty for early jumping
			result = jump_height - early_jump_penalty;
			break;
		}

		if ( negate_result )
			result = -result;

		GetReport().set_value( result );
		GetReport().set( "jump_height", jump_height );
		GetReport().set( "early_jump_penalty", early_jump_penalty );

		return result;
	}

	double JumpMeasure::GetLongJumpResult( const Model& model )
	{
		Vec3 com_pos = model.GetComPos();
		Vec3 com_vel = model.GetComVel();

		double early_jump_penalty = std::max( 0.0, prepare_com.y - init_com.y ) / prepare_time;
		double takeoff_speed = ( com_pos.y - prepare_com.y ) / ( model.GetTime() - prepare_time );
		double com_landing_distance = GetLandingDist( com_pos, com_vel );
		double body_landing_distance = target_body ? GetLandingDist( target_body->GetComPos(), target_body->GetComVel() ) : 1000.0;

		GetReport().set( "early_jump_penalty", early_jump_penalty );
		GetReport().set( "takeoff_speed", takeoff_speed );
		GetReport().set( "com_landing_distance", com_landing_distance );
		GetReport().set( "body_landing_distance", body_landing_distance );

		double result = 0.0;
		switch ( state )
		{
		case scone::JumpMeasure::Prepare:
			// take final com vel in y
			result = com_vel.y;
			break;
		case scone::JumpMeasure::Takeoff:
		{
			result = takeoff_speed - early_jump_penalty;
			break;
		}
		case scone::JumpMeasure::Flight:
		case scone::JumpMeasure::Landing:
		case scone::JumpMeasure::Recover:
		{
			double recover_bonus = 50 + 50 * ( model.GetTime() - recover_start_time ) / recover_time;
			GetReport().set( "recover_bonus", recover_bonus );
			GetReport().set( "recover_cop_dist", recover_cop_dist );
			result = 10 + recover_bonus * ( std::min( { com_landing_distance, body_landing_distance, recover_cop_dist } ) - early_jump_penalty );
			break;
		}
		}

		if ( negate_result ) result = -result;

		GetReport().set_value( result );

		return result;
	}

	double JumpMeasure::GetLandingDist( const Vec3& pos, const Vec3& vel, double floor_height )
	{
		double t = 0.0;
		double g = -9.81;
		double y0 = pos.y - floor_height;
		double disc = vel.y * vel.y - 2 * g * y0;

		if ( disc > 0 )
			t = ( -vel.y - sqrt( vel.y * vel.y - 2 * g * y0 ) ) / g; // polynomial has two roots
		else
			t = ( -vel.y - sqrt( vel.y * vel.y + 2 * g * y0 ) ) / g; // polynomial has one or complex root

		return pos.x + t * vel.x;
	}
}

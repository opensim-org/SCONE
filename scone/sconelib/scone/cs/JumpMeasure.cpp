#include "JumpMeasure.h"
#include "scone/sim/Area.h"

namespace scone
{
	namespace cs
	{
		JumpMeasure::JumpMeasure( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area ) :
		Measure( props, par, model, area ),
		target_body( nullptr ),
		state( Prepraration )
		{
			INIT_PROPERTY( props, termination_height, 0.5 );
			INIT_PROPERTY( props, prepare_time, 0.2 );
			INIT_PROPERTY( props, terminate_on_peak, true );
			INIT_PROPERTY( props, negate_result, false );
			INIT_PROPERTY( props, jump_type, int( HighJump ) );

			if ( props.HasKey( "target_body" ) )
				target_body = FindByName( model.GetBodies(), props.GetStr( "target_body" ) ).get();

			init_com = model.GetComPos();
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
			case scone::cs::JumpMeasure::Prepraration:
				if ( pos.y < termination_height * init_com.y )
					return RequestTermination;

				if ( timestamp >= prepare_time && vel.y > 0 )
				{
					state = Takeoff;
					prepare_com = pos;
				}
				break;
			case scone::cs::JumpMeasure::Takeoff:
				if ( model.GetTotalContactForce() <= 0 )
					state = Flight;
				// falltrough to allow transition to Landing
			case scone::cs::JumpMeasure::Flight:
				if ( vel.y < 0 )
				{
					if ( terminate_on_peak )
						return RequestTermination;
					else state = Landing;
				}
				break;
			case scone::cs::JumpMeasure::Landing:
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
			case scone::cs::JumpMeasure::Prepraration:
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

			// get position of leftmost body
			double min_pos_x = 1000.0;
			for ( auto& body : model.GetBodies() )
				min_pos_x = std::min( min_pos_x, body->GetComPos().x );

			const double no_flight_penalty = 200;
			double early_jump_penalty = 100 * std::max( 0.0, prepare_com.y - init_com.y );
			double jump_height = 100 * pos.y;
			double jump_dist = 100 * ( min_pos_x - init_com.x );

			// compute results based on state
			double result = 0.0;
			switch ( state )
			{
			case scone::cs::JumpMeasure::Prepraration:
				// failed during preparation, return projected height after 1s
				result = 100 * ( init_com.y + ( pos.y - init_com.y ) / model.GetTime() ) - no_flight_penalty;
				break;
			case scone::cs::JumpMeasure::Takeoff:
				// we've managed to take-off, return height as result, with penalty for early jumping
				result = jump_height - early_jump_penalty - no_flight_penalty;
				break;
			case scone::cs::JumpMeasure::Flight:
				// we've managed to take-off, return height as result, with penalty for early jumping
				result = jump_dist;
			}

			if ( negate_result ) result = -result;

			GetReport().Set( result );
			GetReport().Set( "jump_height", jump_height );
			GetReport().Set( "jump_distance", jump_dist );
			GetReport().Set( "early_jump_penalty", early_jump_penalty );

			return result;

		}
	}
}

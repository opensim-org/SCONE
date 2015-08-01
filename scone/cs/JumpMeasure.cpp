#include "stdafx.h"
#include "JumpMeasure.h"
#include "../sim/Area.h"

namespace scone
{
	namespace cs
	{
		JumpMeasure::JumpMeasure( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area ) :
		Measure( props, par, model, area ),
		target_body( *FindByName( model.GetBodies(), props.GetStr( "target_body" ) ) ),
		has_moved_up( false ),
		distance( 0.0 ),
		init_height( model.GetComPos().y ),
		init_dist( std::min( model.GetComPos().x, target_body.GetPos().x ) ),
		prev_force( -1.0 )
		{
			INIT_PROPERTY( props, termination_height, 0.5 );
			INIT_PROPERTY( props, ignore_time, 0.1 );
		}

		JumpMeasure::~JumpMeasure()
		{
		}

		double JumpMeasure::GetResult( sim::Model& model )
		{
			return distance;
		}

		sim::Controller::UpdateResult JumpMeasure::UpdateAnalysis( const sim::Model& model, double timestamp )
		{
			if ( timestamp < ignore_time )
				return SuccessfulUpdate;

			if ( model.GetComPos().y < termination_height * init_height )
				return RequestTermination;

			Real force = model.GetLeg( 0 ).GetContactForce().y + model.GetLeg( 1 ).GetContactForce().y;
			if ( prev_force == -1.0 ) prev_force = force;

			bool move_up = force < prev_force;
			prev_force = force;

			if ( !move_up && has_moved_up )
			{
				// model has landed, update distance
				Real d = 100 * ( std::min( model.GetComPos().x, target_body.GetPos().x ) - init_dist );
				distance = std::max( distance, d );
			}
			else if ( move_up && !has_moved_up )
			{
				// model has gone into flight (or well, is moving up at least)
				has_moved_up = true;
			}
			else if ( has_moved_up && move_up && distance > 0.0 )
			{
				// controller is going for a second jump, stop it!
				return Controller::RequestTermination;
			}

			return Controller::SuccessfulUpdate;
		}

		scone::String JumpMeasure::GetClassSignature() const 
		{
			return "LongJump";
		}
	}
}

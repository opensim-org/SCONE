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
		was_airborne( false ),
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
			{
				init_dist = std::min( model.GetComPos().x, target_body.GetPos().x );
				return SuccessfulUpdate;
			}

			Real y_com = model.GetComPos().y;
			if ( y_com < termination_height * init_height )
				return RequestTermination;

			Real force = model.GetLeg( 0 ).GetContactForce().y + model.GetLeg( 1 ).GetContactForce().y;
			if ( prev_force == -1.0 ) prev_force = force;

			bool contact = force > 0.1;
			bool move_up = force < prev_force;
			bool move_down = force > prev_force;
			prev_force = force;

			if ( contact && was_airborne )
			{
				// model has landed, update distance and terminate
				Real d = 100 * ( std::min( model.GetComPos().x, target_body.GetPos().x ) - init_dist );
				distance = std::max( distance, d );
				return Controller::RequestTermination;
			}
			else if ( !contact && !was_airborne )
			{
				// model has gone into flight
				was_airborne = true;
			}

			return Controller::SuccessfulUpdate;
		}

		scone::String JumpMeasure::GetClassSignature() const 
		{
			return "LongJump";
		}
	}
}

#include "JumpMeasure.h"
#include "scone/sim/Area.h"

namespace scone
{
	namespace cs
	{
		JumpMeasure::JumpMeasure( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area ) :
		Measure( props, par, model, area ),
		target_body( nullptr ),
		state( Preparing )
		{
			INIT_PROPERTY( props, termination_height, 0.5 );
			INIT_PROPERTY( props, prepare_time, 0.2 );
			INIT_PROPERTY( props, terminate_on_peak, true );

			if ( props.HasKey( "target_body" ) )
				target_body = FindByName( model.GetBodies(), props.GetStr( "target_body" ) ).get();

			init_com = model.GetComPos();
		}

		JumpMeasure::~JumpMeasure() { }

		double JumpMeasure::GetResult( sim::Model& model )
		{
			Vec3 pos = target_body ? target_body->GetComPos() : model.GetComPos();

			switch ( state )
			{
			case scone::cs::JumpMeasure::Preparing:
				// failed during preparation, return projected height after 1s
				return 100 * ( init_com.y + ( pos.y - init_com.y ) / model.GetTime() );
				break;
			case scone::cs::JumpMeasure::Jumping:
				// we've managed to jump, return height as result, with penalty for early jumping
				return 100 * ( pos.y - std::max( 0.0, prepare_com.y - init_com.y ) );
				break;
			case scone::cs::JumpMeasure::Landing:
				return 0;
				break;
			}

			return 0;
		}

		sim::Controller::UpdateResult JumpMeasure::UpdateAnalysis( const sim::Model& model, double timestamp )
		{
			if ( !IsActive( model, timestamp ) )
				return NoUpdate;

			Vec3 pos = target_body ? target_body->GetComPos() : model.GetComPos();
			Vec3 vel = target_body ? target_body->GetLinVel() : model.GetComVel();

			switch ( state )
			{
			case scone::cs::JumpMeasure::Preparing:
				if ( pos.y < termination_height * init_com.y )
					return RequestTermination;

				if ( timestamp >= prepare_time && vel.y > 0 )
				{
					state = Jumping;
					prepare_com = pos;
				}
				break;
			case scone::cs::JumpMeasure::Jumping:
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
			return "Jump";
		}
	}
}

/*
** BodyMeasure.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "BodyMeasure.h"
#include "scone/model/Model.h"
#include "scone/core/string_tools.h"
#include "scone/model/Side.h"

namespace scone
{

	BodyMeasure::BodyMeasure( const PropNode& props, Params& par, const Model& model, const Location& loc ) :
	Measure( props, par, model, loc ),
	body( *FindByLocation( model.GetBodies(), props.get< String >( "body" ), loc ) ),
	range_count( 0 )
	{
		INIT_PROP( props, offset, Vec3::zero() );
		INIT_PROP( props, direction, Vec3::zero() );
		INIT_PROP( props, relative_to_model_com, false );
		INIT_PROP( props, magnitude, direction.is_null() );
		INIT_PROP( props, position, RangePenalty<Real>() );
		INIT_PROP( props, velocity, RangePenalty<Real>() );
		INIT_PROP( props, acceleration, RangePenalty<Real>() );

		range_count = int( !position.IsNull() ) + int( !velocity.IsNull() ) + int( !acceleration.IsNull() );
		name = body.GetName();

		// Determine the active state.
		String states;
		props.try_get< String >(states, "states" );
		auto state_tokens = xo::split_str( states, ";," );
		for ( const String& instance_states_str : state_tokens )
		{
			auto instance_states = xo::split_str( instance_states_str, " " );
			for ( const auto& state : instance_states )
				active_state.set(GaitStateController::m_StateNames.GetValue(state));
		}
		if (!active_state.any()) // if states are not defined then make always active
			active_state.flip();
	}

	double BodyMeasure::ComputeResult( const Model& model )
	{
		double penalty = 0.0;
		if ( !position.IsNull() )
		{
			penalty += position.GetResult();
			if ( range_count > 1 )
				GetReport().set( name + ".pos_penalty", stringf( "%g", position.GetResult() ) );
		}
		if ( !velocity.IsNull() )
		{
			penalty += velocity.GetResult();
			if ( range_count > 1 )
				GetReport().set( name + ".vel_penalty", stringf( "%g", velocity.GetResult() ) );
		}
		if ( !acceleration.IsNull() )
		{
			penalty += acceleration.GetResult();
			if ( range_count > 1 )
				GetReport().set( name + ".acc_penalty", stringf( "%g", acceleration.GetResult() ) );
		}

		return  penalty;
	}

	bool BodyMeasure::UpdateMeasure( const Model& model, double timestamp )
	{
		// Determine if the measure is in active state.
		bool isInActiveState = true;
		auto side = (body.GetName().find("_r") != std::string::npos) ? Side::RightSide : Side::LeftSide;
		auto gaitController = dynamic_cast<const GaitStateController*>(model.GetController());
		if (gaitController != nullptr) // check if GaitStateController exists
		{
			const auto& legStates = gaitController->GetLegStates();
			for (const auto& legState : legStates) // iterate over each leg to find the side
				if (legState.get()->leg.GetSide() == side)
					if (!active_state.test(legState->state)) // if not active skip following calculations
						isInActiveState = false;
		}

		if (!isInActiveState)
		{
			// We want to append min in the measures in case we are
			// not in active state. Otherwise, StoreData will report
			// the last value.
			if ( !position.IsNull() )
				position.AddSample( timestamp, position.min );

			if ( !velocity.IsNull() )
				velocity.AddSample( timestamp, velocity.min );

			if ( !acceleration.IsNull() )
				acceleration.AddSample( timestamp, acceleration.min );
		}
		else
		{
			if ( !position.IsNull() )
			{
				auto pos = body.GetPosOfPointOnBody( offset );
				if ( relative_to_model_com ) pos -= model.GetComPos();
				position.AddSample( timestamp, magnitude ? length( pos ) : dot_product( direction, pos ) );
			}

			if ( !velocity.IsNull() )
			{
				auto vel = body.GetLinVelOfPointOnBody( offset );
				if ( relative_to_model_com ) vel -= model.GetComVel();
				velocity.AddSample( timestamp, magnitude ? length( vel ) : dot_product( direction, vel ) );
			}

			if ( !acceleration.IsNull() )
			{
				auto acc = body.GetLinAccOfPointOnBody( offset );
				if ( relative_to_model_com ) acc -= model.GetComAcc();
				acceleration.AddSample( timestamp, magnitude ? length( acc ) : dot_product( direction, acc ) );
			}
		}

		return false;
	}

	String BodyMeasure::GetClassSignature() const
	{
		return String();
	}

	void BodyMeasure::StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const
	{
		if ( !position.IsNull() )
			frame[ body.GetName() + ".pos_penalty" ] = position.GetLatest();
		if ( !velocity.IsNull() )
			frame[ body.GetName() + ".vel_penalty" ] = velocity.GetLatest();
		if ( !acceleration.IsNull() )
			frame[ body.GetName() + ".acc_penalty" ] = acceleration.GetLatest();
	}
}

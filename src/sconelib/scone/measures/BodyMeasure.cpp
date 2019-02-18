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

namespace scone
{

	BodyMeasure::BodyMeasure( const PropNode& props, Params& par, Model& model, const Location& loc ) :
	Measure( props, par, model, loc ),
	body( *FindByLocation( model.GetBodies(), props.get< String >( "body" ), loc ) ),
	range_count( 0 )
	{
		INIT_PROP( props, axes_weights, Vec3::zero() );
		INIT_PROP( props, relative_to_model_com, false );
		INIT_PROP( props, position, RangePenalty< Real >() );
		INIT_PROP( props, velocity, RangePenalty< Real >() );
		INIT_PROP( props, acceleration, RangePenalty< Real >() );

		range_count = int( !position.IsNull() ) + int( !velocity.IsNull() ) + int( !acceleration.IsNull() );
		name = body.GetName();
	}

	double BodyMeasure::ComputeResult( Model& model )
	{
		double penalty = 0.0;
		if ( !position.IsNull() )
		{
			penalty += position.GetAverage();
			if ( range_count > 1 )
				GetReport().set( name + ".pos_limit", stringf( "%g", position.GetAverage() ) );
		}
		if ( !velocity.IsNull() )
		{
			penalty += velocity.GetAverage();
			if ( range_count > 1 )
				GetReport().set( name + ".vel_limit", stringf( "%g", velocity.GetAverage() ) );
		}
		if ( !acceleration.IsNull() )
		{
			penalty += acceleration.GetAverage();
			if ( range_count > 1 )
				GetReport().set( name + ".acc_limit", stringf( "%g", acceleration.GetAverage() ) );
		}

		return  penalty;
	}

	bool BodyMeasure::UpdateMeasure( const Model& model, double timestamp )
	{
		if ( !position.IsNull() )
		{
			auto pos = body.GetPosOfPointOnBody( offset );
			if ( relative_to_model_com ) pos -= model.GetComPos();
			position.AddSample( timestamp, dot_product( axes_weights, pos ) );
		}

		if ( !velocity.IsNull() )
		{
			auto vel = body.GetLinVelOfPointOnBody( offset );
			if ( relative_to_model_com ) vel -= model.GetComVel();
			velocity.AddSample( timestamp, dot_product( axes_weights, vel ) );
		}

		if ( !acceleration.IsNull() )
		{
			auto acc = body.GetLinAccOfPointOnBody( offset );
			if ( relative_to_model_com ) acc -= model.GetComAcc();
			acceleration.AddSample( timestamp, dot_product( axes_weights, acc ) );
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
			frame[ GetName() + ".pos_limit" ] = position.GetLatest();
		if ( !velocity.IsNull() )
			frame[ GetName() + ".vel_limit" ] = velocity.GetLatest();
		if ( !acceleration.IsNull() )
			frame[ GetName() + ".acc_limit" ] = acceleration.GetLatest();
	}
}

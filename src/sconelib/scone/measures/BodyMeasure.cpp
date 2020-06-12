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

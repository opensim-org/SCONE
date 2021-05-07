/*
** DofMeasure.cpp
**
** Copyright (C) 2013-2021 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "DofMeasure.h"
#include "scone/model/Model.h"
#include "scone/core/string_tools.h"

namespace scone
{
	DofMeasure::DofMeasure( const PropNode& props, Params& par, const Model& model, const Location& loc ) :
	Measure( props, par, model, loc ),
	dof( *FindByLocation( model.GetDofs(), props.get< String >( "dof" ), loc ) ),
	parent( nullptr ),
	range_count( 0 )
	{
		if ( props.try_get< String >( "parent" ) )
			parent = FindByLocation( model.GetDofs(), props.get< String >( "parent" ), loc ).get();

		INIT_PROP( props, position, RangePenalty<Degree>() );
		INIT_PROP( props, velocity, RangePenalty<Degree>() );
		INIT_PROP( props, acceleration, RangePenalty<Degree>() );
		INIT_PROP( props, force, RangePenalty<Real>() );

		range_count = int( !position.IsNull() ) + int( !velocity.IsNull() ) + int( !acceleration.IsNull() ) + int( !force.IsNull() );
		if ( name.empty() )
			name = dof.GetName();
	}

	double DofMeasure::ComputeResult( const Model& model )
	{
		double penalty = 0;
		if ( !position.IsNull() )
		{
			penalty += position.GetResult().value;
			if ( range_count > 1 )
				GetReport().set( name + ".position_penalty" , stringf( "%g", position.GetResult() ) );
		}
		if ( !velocity.IsNull() )
		{
			penalty += velocity.GetResult().value;
			if ( range_count > 1 )
				GetReport().set( name + ".velocity_penalty", stringf( "%g", velocity.GetResult() ) );
		}
		if ( !acceleration.IsNull() )
		{
			penalty += acceleration.GetResult().value;
			if ( range_count > 1 )
				GetReport().set( name + ".acceleration_penalty", stringf( "%g", acceleration.GetResult() ) );
		}
		if ( !force.IsNull() )
		{
			penalty += force.GetResult();
			if ( range_count > 1 )
				GetReport().set( name + ".force_penalty", stringf( "%g", force.GetResult() ) );
		}

		return penalty;
	}

	bool DofMeasure::UpdateMeasure( const Model& model, double timestamp )
	{
		if ( !position.IsNull() )
			position.AddSample( timestamp, Degree( Radian( dof.GetPos() + ( parent ? parent->GetPos() : 0 ) ) ) );
		if ( !velocity.IsNull() )
			velocity.AddSample( timestamp, Degree( Radian( dof.GetVel() + ( parent ? parent->GetVel() : 0 ) ) ) );
		if ( !acceleration.IsNull() )
			acceleration.AddSample( timestamp, Degree( Radian( dof.GetAcc() + ( parent ? parent->GetAcc() : 0 ) ) ) );
		if ( !force.IsNull() )
			force.AddSample( timestamp, dof.GetLimitMoment() );
		return false;
	}

	String DofMeasure::GetClassSignature() const
	{
		return String();
	}

	void DofMeasure::StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const
	{
		if ( !position.IsNull() )
			frame[ dof.GetName() + ".position_penalty" ] = position.GetLatest().value;
		if ( !velocity.IsNull() )
			frame[ dof.GetName() + ".velocity_penalty" ] = velocity.GetLatest().value;
		if ( !acceleration.IsNull() )
			frame[ dof.GetName() + ".acceleration_penalty" ] = acceleration.GetLatest().value;
		if ( !force.IsNull() )
			frame[ dof.GetName() + ".force_penalty" ] = force.GetLatest();
	}
}

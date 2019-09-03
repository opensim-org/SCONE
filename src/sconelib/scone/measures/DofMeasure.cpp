/*
** DofMeasure.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "DofMeasure.h"
#include "scone/model/Model.h"
#include "scone/core/string_tools.h"

namespace scone
{
	DofMeasure::DofMeasure( const PropNode& props, Params& par, Model& model, const Location& loc ) :
	Measure( props, par, model, loc ),
	dof( *FindByLocation( model.GetDofs(), props.get< String >( "dof" ), loc ) ),
	parent( nullptr ),
	range_count( 0 )
	{
		if ( props.try_get< String >( "parent" ) )
			parent = FindByLocation( model.GetDofs(), props.get< String >( "parent" ), loc ).get();

		INIT_PROP( props, position, RangePenalty< Degree >() );
		INIT_PROP( props, velocity, RangePenalty< Degree >() );
		INIT_PROP( props, force, RangePenalty< double >() );

		range_count = int( !position.IsNull() ) + int( !velocity.IsNull() ) + int( !force.IsNull() );
		if ( name.empty() )
			name = dof.GetName();
	}

	double DofMeasure::ComputeResult( Model& model )
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
		position.AddSample( timestamp, Radian( dof.GetPos() + ( parent ? parent->GetPos() : 0 ) ) );
		velocity.AddSample( timestamp, Radian( dof.GetVel() + ( parent ? parent->GetVel() : 0 ) ) );
		force.AddSample( timestamp, dof.GetLimitForce() );
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
		if ( !force.IsNull() )
			frame[ dof.GetName() + ".force_penalty" ] = force.GetLatest();
	}
}

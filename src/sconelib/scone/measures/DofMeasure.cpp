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
			parent = &*FindByLocation( model.GetDofs(), props.get< String >( "parent" ), loc );

		INIT_PROP( props, position, RangePenalty< Degree >() );
		INIT_PROP( props, velocity, RangePenalty< Degree >() );
		INIT_PROP( props, force, RangePenalty< double >() );

		range_count = int( !position.IsNull() ) + int( !velocity.IsNull() ) + int( !force.IsNull() );
		name = dof.GetName();
	}

	double DofMeasure::ComputeResult( Model& model )
	{
		double penalty = 0.0;
		if ( !position.IsNull() )
		{
			penalty += position.GetAverage();
			if ( range_count > 1 )
				GetReport().set( name + ".pos_limit" , stringf( "%g", position.GetAverage() ) );
		}
		if ( !velocity.IsNull() )
		{
			penalty += velocity.GetAverage();
			if ( range_count > 1 )
				GetReport().set( name + ".vel_limit", stringf( "%g", velocity.GetAverage() ) );
		}
		if ( !force.IsNull() )
		{
			penalty += force.GetAverage();
			if ( range_count > 1 )
				GetReport().set( name + ".force_limit", stringf( "%g", force.GetAverage() ) );
		}

		return  penalty;
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
			frame[ dof.GetName() + ".pos_limit" ] = position.GetLatest();
		if ( !velocity.IsNull() )
			frame[ dof.GetName() + ".vel_limit" ] = velocity.GetLatest();
		if ( !force.IsNull() )
			frame[ dof.GetName() + ".force_limit" ] = force.GetLatest();
	}
}

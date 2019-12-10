/*
** MuscleMeasure.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "MuscleMeasure.h"
#include "scone/model/Model.h"
#include "scone/core/string_tools.h"

namespace scone
{
	MuscleMeasure::MuscleMeasure( const PropNode& props, Params& par, const Model& model, const Location& loc ) :
		Measure( props, par, model, loc ),
		muscle( *FindByLocation( model.GetMuscles(), props.get< String >( "muscle" ), loc ) ),
		range_count( 0 )
	{
		INIT_PROP( props, activation, RangePenalty<Real>() );
		INIT_PROP( props, length, RangePenalty<Real>() );
		INIT_PROP( props, velocity, RangePenalty<Real>() );

		range_count = int( !activation.IsNull() ) + int( !velocity.IsNull() ) + int( !length.IsNull() );
		if ( name.empty() )
			name = muscle.GetName();
	}

	double MuscleMeasure::ComputeResult( const Model& model )
	{
		double penalty = 0.0;
		if ( !activation.IsNull() )
		{
			penalty += activation.GetResult();
			if ( range_count > 1 )
				GetReport().set( name + ".activation_penalty", stringf( "%g", activation.GetResult() ) );
		}
		if ( !velocity.IsNull() )
		{
			penalty += velocity.GetResult();
			if ( range_count > 1 )
				GetReport().set( name + ".velocity_penalty", stringf( "%g", velocity.GetResult() ) );
		}
		if ( !length.IsNull() )
		{
			penalty += length.GetResult();
			if ( range_count > 1 )
				GetReport().set( name + ".length_penalty", stringf( "%g", length.GetResult() ) );
		}

		return  penalty;
	}

	bool MuscleMeasure::UpdateMeasure( const Model& model, double timestamp )
	{
		activation.AddSample( timestamp, muscle.GetActivation() );
		velocity.AddSample( timestamp, muscle.GetNormalizedFiberVelocity() );
		length.AddSample( timestamp, muscle.GetNormalizedFiberLength() );
		return false;
	}

	String MuscleMeasure::GetClassSignature() const
	{
		return String();
	}

	void MuscleMeasure::StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const
	{
		if ( !activation.IsNull() )
			frame[ muscle.GetName() + ".activation_penalty" ] = activation.GetLatest();
		if ( !velocity.IsNull() )
			frame[ muscle.GetName() + ".velocity_penalty" ] = velocity.GetLatest();
		if ( !length.IsNull() )
			frame[ muscle.GetName() + ".length_penalty" ] = length.GetLatest();
	}
}

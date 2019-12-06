/*
** JointLoadMeasure.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "JointLoadMeasure.h"

#include "scone/model/Model.h"

namespace scone
{
	JointLoadMeasure::JointLoadMeasure( const PropNode& props, Params& par, const Model& model, const Location& loc ) :
		Measure( props, par, model, loc ),
		RangePenalty( props ),
		joint_load(),
		joint( *FindByName( model.GetJoints(), props.get< String >( "joint" ) ) )
	{
		INIT_PROP( props, method, 1 );
	}

	double JointLoadMeasure::ComputeResult( const Model& model )
	{
		return RangePenalty<Real>::GetResult();
	}

	bool JointLoadMeasure::UpdateMeasure( const Model& model, double timestamp )
	{
		joint_load = joint.GetLoad();
		AddSample( timestamp, joint_load );

		return false;
	}

	scone::String JointLoadMeasure::GetClassSignature() const
	{
		return "";
	}

	void JointLoadMeasure::StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const
	{
		// #todo: store joint load value
		frame[ joint.GetName() + ".load_penalty" ] = GetLatest();
	}
}

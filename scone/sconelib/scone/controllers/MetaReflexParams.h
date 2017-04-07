#pragma once

#include "scone/core/PropNode.h"
#include "scone/core/Angle.h"

#include "scone/optimization/ParamSet.h"
#include "scone/model/Model.h"

namespace scone
{
	struct MetaReflexParams
	{
		MetaReflexParams() : ref_pos( 0 ), length_gain( 0 ), force_gain( 0 ), constant( 0 ), stiffness( 0 ), symmetry( 0 ) {}
		MetaReflexParams( const PropNode& props, ParamSet& par, Model& model, const String& parname_postfix = "" );
		void Init( const PropNode& props, ParamSet& par, Model& model, const String& parname_postfix = "" );

		Degree ref_pos;
		Real length_gain;
		Real force_gain;
		Real constant;
		Real symmetry;
		Real stiffness;
	};
}

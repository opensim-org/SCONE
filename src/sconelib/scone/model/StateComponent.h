/*
** StateComponent.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "scone/core/Exception.h"
#include "scone/core/platform.h"
#include "scone/core/types.h"
#include "scone/core/HasName.h"
#include "scone/optimization/Params.h"
#include "scone/core/PropNode.h"
#include <vector>

namespace scone
{
	/// The StateComponent is used to define components that contain
	/// differential equations. The user must define a function that
	/// returns the initial condition and the calculation of the state
	/// derivative.
	class SCONE_API StateComponent : public HasName
	{
	public:
		StateComponent( const PropNode& props, Params& par )
			: INIT_MEMBER_REQUIRED( props, name ) {};

		/// Name of the state component. Required parameter.
		String name;
		virtual const String& GetName() const override { return name; }

		/// interface
		virtual std::vector< Real > getInitialCondition() const { SCONE_THROW_NOT_IMPLEMENTED }
		virtual std::vector< Real > calcStateDerivatives( Real t, std::vector< Real > x0 ) const
		{ SCONE_THROW_NOT_IMPLEMENTED}
	};
}

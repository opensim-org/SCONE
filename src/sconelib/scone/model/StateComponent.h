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
#include <vector>

namespace scone
{
	/// The StateComponent is used to include differential equations into OpenSim's model.
	class SCONE_API StateComponent : public HasName
	{
	public:
		StateComponent() {};
		virtual ~StateComponent() {};

		// Name of the state component
		String name;

		virtual std::vector< Real > getInitialCondition() const { SCONE_THROW_NOT_IMPLEMENTED };
		virtual std::vector< Real > calcStateDerivatives( Real t, std::vector< Real > x0 ) const
		{ SCONE_THROW_NOT_IMPLEMENTED};
		virtual const String& GetName() const override { return name; }
	};
}

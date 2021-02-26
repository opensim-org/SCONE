/*
** Function.h
**
** Copyright (C) 2013-2021 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "scone/core/Exception.h"
#include "scone/core/platform.h"
#include "types.h"

namespace scone
{
	/// Abstract parameterizable function, see subclasses for actual implementations.
	class SCONE_API Function
	{
	public:
		Function() { };
		virtual ~Function() { };

		virtual Real GetValue( Real x ) = 0;
		virtual Real GetDerivativeValue( Real x ) { SCONE_THROW_NOT_IMPLEMENTED };
		virtual String GetSignature() { return "UnknownFunction"; }
	};
}

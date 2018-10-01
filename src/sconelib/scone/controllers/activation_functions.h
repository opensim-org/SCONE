/*
** activation_functions.h
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include <algorithm>
#include <functional>
#include "scone/core/platform.h"
#include "scone/core/types.h"

namespace scone
{
	using activation_func_t = std::function< double( double ) >;

	activation_func_t GetActivationFunction( const String& name );
	double rectifier( double input );
	double soft_plus( double input );
	double linear( double input );
	double gaussian( double input );
	double gaussian_width( double input, double width );
}

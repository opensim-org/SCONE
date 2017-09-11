#pragma once

#include <algorithm>
#include <functional>
#include "../core/core.h"

namespace scone
{
	std::function< double( double ) > GetActivationFunction( const String& name );
	double rectifier( double input );
	double soft_plus( double input );
	double linear( double input );
}

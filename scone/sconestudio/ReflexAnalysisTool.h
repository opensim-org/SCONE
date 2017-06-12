#pragma once

#include "scone/core/Storage.h"
#include "flut/optimization/objective.hpp"

namespace scone
{
	class ReflexAnalysisTool : public flut::objective
	{
	public:
		ReflexAnalysisTool( const Storage< Real >& data ) {}

	protected:
	private:
	};
}

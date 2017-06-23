#pragma once

#include "scone/core/Storage.h"
#include "scone/core/types.h"
#include "spot/objective.h"
#include "flut/storage.hpp"

namespace scone
{
	class ReflexAnalysisObjective : public spot::objective
	{
	public:
		ReflexAnalysisObjective( const Storage< Real >& data );
		virtual spot::fitness_t evaluate( const spot::search_point& point ) const override;

	private:
		flut::storage< double > excitations_;
		flut::storage< double > signals_;
	};
}

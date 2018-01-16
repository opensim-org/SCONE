#pragma once

#include "Params.h"
#include "scone/core/propnode_tools.h"
#include "scone/core/HasSignature.h"
#include "spot/objective.h"
#include "scone/core/system_tools.h"

namespace scone
{
	using spot::fitness_t;

	class SCONE_API Objective : public HasSignature, public spot::objective
	{
	public:
		Objective( const PropNode& props );
		virtual ~Objective();

		// write results and return all files written
		virtual std::vector< String > WriteResults( const String& file_base ) { return std::vector< String >(); }
		virtual std::vector< path > GetExternalFiles() const { return std::vector< path >(); }
	};
}

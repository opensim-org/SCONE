#pragma once

#include "ParamSet.h"
#include "scone/core/propnode_tools.h"
#include "scone/core/HasSignature.h"
#include "flut/optimization/objective.hpp"

namespace scone
{
	using flut::fitness_t;
	using std::vector;
	using std::string;

	class SCONE_API Objective : public HasSignature, public flut::objective
	{
	public:
		Objective( const PropNode& props );
		virtual ~Objective();

		// write results and return all files written
		virtual std::vector< String > WriteResults( const String& file_base ) { return std::vector< String >(); }

		virtual bool minimize() const override { return minimize_; }
		void set_minimize( bool m ) { minimize_ = m; }

	private:
		bool minimize_;
	};
}

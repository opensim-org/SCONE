#pragma once

#include "ParamSet.h"
#include "scone/core/propnode_tools.h"
#include "scone/core/HasSignature.h"
#include "flut/optimization/objective.hpp"

namespace scone
{
	using flut::par_bounds;
	using flut::par_mean_std;
	using flut::fitness_t;
	using std::vector;
	using std::string;

	class SCONE_API Objective : public HasSignature, public flut::objective
	{
	public:
		Objective( const PropNode& props, ParamSet& par );
		virtual ~Objective();

		// get all parameters in this objective
		ParamSet MakeParamSet();

		// update all parameters and call Evaluate
		double Evaluate( ParamSet& par );

		// virtual evaluation function
		virtual double Evaluate() = 0;

		// write results and return all files written
		virtual std::vector< String > WriteResults( const String& file_base ) { return std::vector< String >(); }

		int debug_idx;

		virtual bool minimize() const override { return minimize_; }
		void set_minimize( bool m ) { minimize_ = m; }

		virtual flut::fitness_t evaluate( const flut::par_vec& point ) const override;

		virtual size_t dim() const override;
		virtual const vector< par_bounds >& parameter_bounds() const override;
		virtual const vector< par_mean_std >& parameter_init() const override;
		virtual const vector< string >& parameter_names() const override;

	protected:
		// process parameters
		virtual void ProcessParameters( ParamSet& par ) = 0;

	private:
		bool minimize_;
	};
}

#include "Objective.h"
#include "scone/core/Exception.h"

namespace scone
{
	Objective::Objective( const PropNode& props, ParamSet& par ) :
	HasSignature( props ),
	debug_idx( -1 ),
	minimize_( true )
	{}

	Objective::~Objective()
	{}

	scone::ParamSet Objective::MakeParamSet()
	{
		ParamSet par( ParamSet::ConstructionMode );
		ProcessParameters( par );
		return par;
	}

	double Objective::Evaluate( ParamSet& par )
	{
		par.SetMode( ParamSet::UpdateMode );
		ProcessParameters( par );

		return Evaluate();
	}

	flut::fitness_t Objective::evaluate( const flut::par_vec& point ) const
	{
		throw std::logic_error( "The method or operation is not implemented." );
	}

	size_t Objective::dim() const
	{
		throw std::logic_error( "The method or operation is not implemented." );
	}

	const vector<flut::par_bounds>& Objective::parameter_bounds() const
	{
		throw std::logic_error( "The method or operation is not implemented." );
	}

	const vector<flut::par_mean_std>& Objective::parameter_init() const
	{
		throw std::logic_error( "The method or operation is not implemented." );
	}

	const vector<flut::string>& Objective::parameter_names() const
	{
		throw std::logic_error( "The method or operation is not implemented." );
	}

}

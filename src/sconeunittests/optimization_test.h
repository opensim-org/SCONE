#include "scone/optimization/opt_tools.h"
#include "scone/optimization/Objective.h"

#include "scone/core/Factories.h"
#include "scone/core/math.h"
#include "xo/filesystem/path.h"

#include "xo/diagnose/test_framework.h"
#include "xo/filesystem/filesystem.h"

using namespace scone;

class TestObjective : public scone::Objective
{
public:
	TestObjective( const PropNode& props ) : Objective( props ), num_params( 0 ), is_evaluating( false )
	{
		INIT_PROP( props, num_params, 0 );
		for ( size_t i = 0; i < num_params; ++i )
			info().add( ParInfo( stringf( "Param%d", i ), 1.0, 0.1, -1000.0, 1000.0 ) );
	}

	double Rosenbrock( const spot::par_vec& v ) const
	{
		// Rosenbrock function
		double sum = 0.0;
		for( unsigned int i = 0; i < v.size()-1; i++ ) {
			sum += 100 * GetSquared( v[i+1] - GetSquared( v[ i ] ) ) + GetSquared( 1. - v[ i ] );
		}
		return sum;
	}

protected:
	virtual double evaluate( const SearchPoint& values ) const override
	{
		SCONE_ASSERT( is_evaluating == false ); // thread safety check

		is_evaluating = true;
		double result = Rosenbrock( values.values() );
		is_evaluating = false;

		return result;
	}

	virtual String GetClassSignature() const override { return "Test"; }

private:
	int num_params;
	std::vector< double > params;
	mutable bool is_evaluating;
};

void optimization_test()
{
	GetObjectiveFactory().register_class< TestObjective >();

	xo::current_path( scone::GetFolder( scone::SCONE_ROOT_FOLDER ) / "unittestdata/optimization_test" );
	const PropNode pn = load_file( "rosenbrock_50_test.xml" );
	OptimizerUP o = CreateOptimizer( pn.get_child( "Optimizer" ) );
	xo::log_unaccessed( pn );
	o->Run();

	XO_TEST( o->GetBestFitness() < 0.1 );
}

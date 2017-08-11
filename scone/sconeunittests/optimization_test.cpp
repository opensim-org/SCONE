#include "common.h"

#include <boost/filesystem.hpp>

#include "scone/optimization/opt_tools.h"
#include "scone/optimization/Objective.h"

#include "scone/core/Factories.h"
#include "scone/core/math.h"
#include "flut/system/path.hpp"

using namespace boost::filesystem;
using namespace scone;

class TestObjective : public scone::Objective
{
public:
	TestObjective( const PropNode& props ) : Objective( props ), num_params( 0 ), is_evaluating( false )
	{
		INIT_PROPERTY( props, num_params, 0 );
		for ( size_t i = 0; i < num_params; ++i )
			info().add( stringf( "Param%d", i ), 1.0, 0.1, -1000.0, 1000.0 );
	}

	static double Rosenbrock( const std::vector< double >& v )
	{
		// Rosenbrock function
		double sum = 0.0;
		for( unsigned int i = 0; i < v.size()-1; i++ ) {
			sum += 100 * GetSquared( v[i+1] - GetSquared( v[ i ] ) ) + GetSquared( 1. - v[ i ] );
		}
		return sum;
	}

protected:
	virtual double evaluate( const ParamInstance& values ) const override
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

BOOST_AUTO_TEST_CASE( optimization_test )
{
	GetObjectiveFactory().register_class< TestObjective >();

	const PropNode pn = load_xml( scone::GetFolder( scone::SCONE_ROOT_FOLDER ) / "unittestdata/optimization_test/rosenbrock_50_test.xml" );
	OptimizerUP o = CreateOptimizer( pn.get_child( "Optimizer" ) );
	LogUntouched( pn );
	o->SetConsoleOutput( false );
	o->Run();

	BOOST_CHECK( o->GetBestFitness() < 0.1 );
}

#include "common.h"

#include <boost/filesystem.hpp>

#include "scone/opt/opt_tools.h"
#include "scone/opt/Objective.h"
#include "scone/opt/Factories.h"


using namespace boost::filesystem;
using namespace scone;

class TestObjective : public scone::opt::Objective
{
public:
	TestObjective( const PropNode& props, opt::ParamSet& par ) : Objective( props, par ), num_params( 0 ), is_evaluating( false )
	{
		INIT_PROPERTY( props, num_params, 0 );
		params.resize( num_params );
	}

	virtual void ProcessParameters( opt::ParamSet& par ) override
	{
		for ( size_t i = 0; i < params.size(); ++i )
			params[ i ] = par.GetMeanStd( stringf( "par%d", i), 1.0, 0.1, -1000.0, 1000.0 );
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
	virtual double Evaluate() override
	{
		SCONE_ASSERT( is_evaluating == false ); // thread safety check
		is_evaluating = true; 
		double result = Rosenbrock( params );
		is_evaluating = false;
		return result;
	}

	virtual String GetClassSignature() const override { return "Test"; }

private:
	int num_params;
	std::vector< double > params;
	bool is_evaluating;
};

BOOST_AUTO_TEST_CASE( optimization_test )
{
	opt::GetObjectiveFactory().Register< TestObjective >();

	auto pn = ReadPropNodeFromXml( scone::GetSconeFolder( "root" ) + "/unittestdata/optimization_test/rosenbrock_50_test.xml" );
	opt::OptimizerUP o = opt::CreateOptimizer( pn.GetChild( "Optimizer" ) );
	LogUntouched( pn );
	o->SetConsoleOutput( false );
	o->Run();

	BOOST_CHECK( o->GetBestFitness() < 0.1 );
}

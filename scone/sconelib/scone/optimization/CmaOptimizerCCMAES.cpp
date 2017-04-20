#include "CmaOptimizerCCMAES.h"
#include "flut/timer.hpp"
#include "flut/optimizer/cma_optimizer.hpp"
#include "flut/container_tools.hpp"
#include <numeric>
#include <random>
#include "flut/string_tools.hpp"

using std::cout;
using std::endl;

namespace scone
{
	CmaOptimizerCCMAES::CmaOptimizerCCMAES( const PropNode& props ) : CmaOptimizer( props )
	{
	}

	void CmaOptimizerCCMAES::Run()
	{
		// make sure there is at least 1 objective and get info
		CreateObjectives( 1 );
		ParamSet par = GetObjective().MakeParamSet();
		size_t dim = par.GetFreeParamCount();

		SCONE_ASSERT( dim > 0 );

		// init random seed
		if ( random_seed == 0 ) {
			std::random_device rd;
			random_seed = rd();
		}

		// initialize settings from file
		if ( use_init_file && !init_file.empty() )
			par.Read( init_file );

		par.SetMode( ParamSet::UpdateMode );

		// generate random initial population
		std::vector< double > initPoint( dim );
		std::vector< double > initStd( dim );
		std::vector< double > lowerBounds( dim );
		std::vector< double > upperBounds( dim );
		size_t free_idx = 0;
		for ( size_t par_idx = 0; par_idx < par.GetParamCount(); ++par_idx )
		{
			auto& parinf = par.GetParamInfo( par_idx );
			if ( parinf.is_free )
			{
				SCONE_ASSERT( free_idx < dim );
				initPoint[ free_idx ] = parinf.init_mean;
				lowerBounds[ free_idx ] = parinf.min;
				upperBounds[ free_idx ] = parinf.max;

				double par_std = parinf.init_std;

				// compute std using global std settings (if they are set)
				if ( global_std_offset != 0.0 || global_std_factor != 0.0 )
					par_std = global_std_factor * fabs( parinf.init_mean ) + global_std_offset;

				initStd[ free_idx ] = par_std;
				++free_idx;
			}
		}

		// init CMA object
		flut::cma_optimizer cma( (int)dim, flut::optimizer::no_objective_func, initPoint, initStd, lowerBounds, upperBounds, m_Lambda, random_seed );
		cma.set_maximize( !IsMinimizing() );
		m_Lambda = cma.lambda();
		m_Mu = cma.mu();
		m_Sigma = cma.sigma();

		// create m_Lambda objectives
		CreateObjectives( m_Lambda );
		GetObjective().GetSignature();

		log::InfoF( "Starting optimization, dim=%d, lambda=%d, mu=%d", dim, m_Lambda, m_Mu );

		if ( GetStatusOutput() )
		{
			// print out some info
			OutputStatus( "folder", AcquireOutputFolder() );
			OutputStatus( "dim", dim );
			OutputStatus( "sigma", m_Sigma );
			OutputStatus( "lambda", m_Lambda );
			OutputStatus( "mu", m_Mu );
			OutputStatus( "max_generations", max_generations );
		}

		// optimization loop
		flut::timer tmr;
		m_BestFitness = IsMinimizing() ? REAL_MAX : REAL_LOWEST;
		for ( size_t gen = 0; gen < max_generations; ++gen )
		{
			if ( GetProgressOutput() )
				printf( "%04d (S=%.3f):", int( gen ), cma.sigma() ); // MSVC2013 doesn't support %zu

			// sample parameter sets
			auto& pop = cma.sample_population();

			par.SetMode( ParamSet::UpdateMode );
			std::vector< ParamSet > parsets( m_Lambda, par );
			for ( size_t ind_idx = 0; ind_idx < m_Lambda; ++ind_idx )
				parsets[ ind_idx ].SetFreeParamValues( pop[ ind_idx ] );

			auto results = Evaluate( parsets );
			auto current_best_it = cma.maximize() ? flut::max_element( results ) : flut::min_element( results );
			size_t current_best_idx = current_best_it - results.begin();
			auto current_best = *current_best_it;
			auto current_avg_fitness = std::accumulate( results.begin(), results.end(), 0.0 ) / results.size();

			// report results
			if ( GetProgressOutput() )
				printf( " A=%.3f", current_avg_fitness );

			if ( GetStatusOutput() )
				OutputStatus( "generation", flut::stringf( "%d %f %f", gen, current_avg_fitness, current_best ) );

			bool new_best = IsBetterThan( current_best, m_BestFitness );
			if ( new_best )
			{
				m_BestFitness = current_best;

				if ( GetProgressOutput() )
					printf( " B=%.3f", m_BestFitness );
				if ( GetStatusOutput() )
					OutputStatus( "best", m_BestFitness );
			}

			if ( new_best || ( gen - m_LastFileOutputGen > max_generations_without_file_output ) )
			{
				// copy best solution to par
				par.SetFreeParamValues( pop[ current_best_idx ] );

				// update mean / std
				par.UpdateMeanStd( cma.current_mean(), cma.current_std() );

				// update best params after mean / std have been updated
				if ( new_best )
					m_BestParams = par;

				m_LastFileOutputGen = gen;

				// write .par file
				String ind_name = flut::stringf( "%04d_%.3f_%.3f", gen, current_avg_fitness, current_best );
				String file_base = AcquireOutputFolder() + ind_name;
				std::vector< String > outputFiles;
				par.Write( file_base + ".par" );
				outputFiles.push_back( file_base + ".par" );

				// cleanup superfluous output files
				if ( new_best )
					ManageFileOutput( m_BestFitness, outputFiles );
			}

			// show time if needed
			if ( GetProgressOutput() )
			{
				if ( show_optimization_time )
					printf( " T=%.1f", tmr.seconds() );
				printf( new_best ? "\n" : "\r" ); // only start newline if there's been a new best
			}

			cma.update_distribution( results );
		}
		if ( console_output )
			cout << "Optimization finished" << endl;

		if ( GetStatusOutput() )
			OutputStatus( "finished", 1 );
	}
}

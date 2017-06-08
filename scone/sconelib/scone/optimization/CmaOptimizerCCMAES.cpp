#include "CmaOptimizerCCMAES.h"
#include "flut/timer.hpp"
#include "flut/optimization/cma_optimizer.hpp"
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
		size_t dim = GetObjective().dim();

		SCONE_ASSERT( dim > 0 );

		// init random seed
		if ( random_seed == 0 ) {
			std::random_device rd;
			random_seed = rd();
		}

		// initialize settings from file
		if ( use_init_file && !init_file.empty() )
		{
			GetObjective().info().import_mean_std( init_file, use_init_file_std );
		}

		if ( global_std_offset != 0.0 || global_std_factor != 0.0 )
		{
			GetObjective().info().set_global_std( global_std_factor, global_std_offset );
		}

		//flut::function_objective obj( dim, []( const flut::param_vec_t& p ) -> flut::fitness_t { flut_error( "No objective defined" ); return 0.0; } );
		GetObjective().set_minimize( IsMinimizing() );

		// init CMA object
		flut::cma_optimizer cma( GetObjective(), m_Lambda, random_seed );
		m_Lambda = cma.lambda();
		m_Mu = cma.mu();
		m_Sigma = cma.sigma();
		cma.set_max_threads( (int)max_threads );

		// create m_Lambda objectives
		//CreateObjectives( m_Lambda );
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

			auto results = cma.evaluate( pop );

			//std::vector< ParamSet > parsets( m_Lambda, par );
			//for ( size_t ind_idx = 0; ind_idx < m_Lambda; ++ind_idx )
			//	parsets[ ind_idx ].SetFreeParamValues( pop[ ind_idx ] );
			//auto results = Evaluate( parsets );

			auto current_best_it = GetObjective().maximize() ? flut::max_element( results ) : flut::min_element( results );
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
				ParamInfo parinf( GetObjective().info() );
				parinf.set_mean_std( cma.current_mean(), cma.current_std() );
				ParamInstance par( parinf, pop[ current_best_idx ] );

				m_LastFileOutputGen = gen;

				// write .par file
				String ind_name = flut::stringf( "%04d_%.3f_%.3f", gen, current_avg_fitness, current_best );
				String file_base = AcquireOutputFolder() + ind_name;
				std::vector< String > outputFiles;
				std::ofstream( file_base + ".par" ) << par;
				//par.write( file_base + ".par" );
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

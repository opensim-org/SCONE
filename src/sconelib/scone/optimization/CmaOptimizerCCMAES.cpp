#include "CmaOptimizerCCMAES.h"
#include "xo/time/timer.h"
#include "spot/cma_optimizer.h"
#include "xo/container/container_tools.h"
#include <numeric>
#include <random>
#include "xo/string/string_tools.h"
#include "scone/core/math.h"
#include "xo/system/log_sink.h"
#include "xo/system/log.h"
#include "scone/core/Exception.h"

using std::cout;
using std::endl;

namespace scone
{
	CmaOptimizerCCMAES::CmaOptimizerCCMAES( const PropNode& props ) : CmaOptimizer( props )
	{
	}

	void CmaOptimizerCCMAES::Run()
	{
		try
		{
			// get objective dim (after static params are handled)
			size_t dim = GetObjective().dim();
			SCONE_ASSERT( dim > 0 );

			//xo::function_objective obj( dim, []( const xo::param_vec_t& p ) -> xo::fitness_t { xo_error( "No objective defined" ); return 0.0; } );
			GetObjective().info().set_minimize( IsMinimizing() );

			// init CMA object
			spot::cma_optimizer cma( GetObjective(), lambda_, random_seed );
			lambda_ = cma.lambda();
			mu_ = cma.mu();
			sigma_ = cma.sigma();
			cma.set_max_threads( (int)max_threads );
			cma.enable_fitness_tracking( window_size );

			// start optimization
			xo::log::file_sink log_sink( xo::log::info_level, AcquireOutputFolder() / "optimization.log" );
			log::InfoF( "Starting optimization, dim=%d, lambda=%d, mu=%d", dim, lambda_, mu_ );

			if ( GetStatusOutput() )
			{
				// print out some info
				OutputStatus( "folder", AcquireOutputFolder() );
				OutputStatus( "dim", dim );
				OutputStatus( "sigma", sigma_ );
				OutputStatus( "lambda", lambda_ );
				OutputStatus( "mu", mu_ );
				OutputStatus( "max_generations", max_generations );
				OutputStatus( "window_size", window_size );
			}

			// setup history.txt
			std::ofstream history_str( ( AcquireOutputFolder() / "history.txt" ).string() );
			history_str << "Step\tBest\tAverage\tPredicted\tSlope\tOffset" << std::endl;

			// optimization loop
			xo::timer tmr;
			m_BestFitness = IsMinimizing() ? REAL_MAX : REAL_LOWEST;
			for ( size_t gen = 0; gen < max_generations; ++gen )
			{
				if ( GetProgressOutput() )
					printf( "%04d (S=%.3f):", int( gen ), cma.sigma() ); // MSVC2013 doesn't support %zu

				// sample parameter sets
				cma.step();
				auto results = cma.current_step_fitnesses();

				// analyze results
				auto current_best_it = GetObjective().info().maximize() ? std::max_element( results.begin(), results.end() ) : std::min_element( results.begin(), results.end() );
				size_t current_best_idx = current_best_it - results.begin();
				auto current_best = *current_best_it;
				auto current_avg_fitness = xo::top_average( results, mu_ );
				auto current_med_fitness = xo::median( results );
				auto cur_trend = cma.fitness_trend();

				// report results
				if ( GetProgressOutput() )
					printf( " A=%.3f O=%.3f S=%.3f", current_avg_fitness, cur_trend.offset(), cur_trend.slope() );

				if ( GetStatusOutput() )
					OutputStatus( "generation", xo::stringf( "%d %g %g %g %g %g", gen, current_best, current_med_fitness, current_avg_fitness, cur_trend.offset(), cur_trend.slope() ) );

				// update history
				history_str << gen << "\t" << current_best << "\t" << current_avg_fitness << "\t" << cma.predicted_fitness( max_generations - cma.current_step() ) << "\t" << cur_trend.slope() << "\t" << cur_trend.offset() << "\n";
				if ( gen % 10 == 9 ) // flush after 10 entries
					history_str.flush();

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
					ParamInstance par( parinf, cma.current_step_best_point().values() );

					m_LastFileOutputGen = gen;

					// write .par file
					String ind_name = xo::stringf( "%04d_%.3f_%.3f", gen, current_avg_fitness, current_best );
					auto file_base = AcquireOutputFolder() / ind_name;
					std::vector< path > outputFiles;
					std::ofstream( ( file_base + ".par" ).str() ) << par;
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
		catch ( std::exception& e )
		{
			if ( GetStatusOutput() )
				OutputStatus( "error", e.what() );
		}
	}
}

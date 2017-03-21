#include "CmaOptimizerShark3.h"

#include "SharkHelperClasses.h"
#include <random>
#include "flut/timer.hpp"
#include "flut/string_tools.hpp"

namespace scone
{
	namespace opt
	{
		CmaOptimizerShark3::CmaOptimizerShark3( const PropNode& props ) : CmaOptimizer( props )
		{

		}

		void CmaOptimizerShark3::Run()
		{
			// make sure there is at least 1 objective and get info
			CreateObjectives( 1 );
			ParamSet par = GetObjective().MakeParamSet();
			size_t dim = par.GetFreeParamCount();

			SCONE_ASSERT( dim > 0 );

			// init lambda and mu
			if ( m_Lambda == 0 ) m_Lambda = static_cast<int>( shark::CMA::suggestLambda( dim ) );
			if ( m_Mu == 0 ) m_Mu = static_cast<int>( shark::CMA::suggestMu( m_Lambda ) );

			// create m_Lambda objectives
			CreateObjectives( m_Lambda );
			GetObjective().GetSignature();

			// init parents and offspring
			CMA_MT cma( *this );
			SconeSingleObjectiveFunction objfunc( GetObjective(), IsMinimizing() );
			cma.recombinationType() = shark::CMA::RecombinationType( recombination_type );

			// init random seed
			if ( random_seed == 0 ) {
				std::random_device rd;
				random_seed = rd();
			}
			shark::Rng::seed( random_seed );

			// initialize settings from file
			if ( use_init_file && !init_file.empty() )
				par.Read( init_file );

			par.SetMode( ParamSet::UpdateMode );

			// generate random initial population
			shark::CMA::SearchPointType initPoint( dim );
			shark::RealMatrix initCovar( dim, dim, 0.0 );
			size_t free_idx = 0;
			for ( size_t par_idx = 0; par_idx < par.GetParamCount(); ++par_idx )
			{
				auto& parinf = par.GetParamInfo( par_idx );
				if ( parinf.is_free )
				{
					SCONE_ASSERT( free_idx < dim );
					initPoint[ free_idx ] = parinf.init_mean;
					double par_std = parinf.init_std;

					// compute std using global std settings (if they are set)
					if ( global_std_offset != 0.0 || global_std_factor != 0.0 )
						par_std = global_std_factor * fabs( parinf.init_mean ) + global_std_offset;

					initCovar( free_idx, free_idx ) = par_std * par_std;
					++free_idx;
				}
			}

			// init CMA object
			log::InfoF( "Starting optimization, dim=%d, lambda=%d, mu=%d", dim, m_Lambda, m_Mu );
			cma.init( objfunc, initPoint, m_Lambda, m_Mu, m_Sigma, initCovar );

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
					printf( "%04d:", int( gen ) ); // MSVC2013 doesn't support %zu

				cma.step_mt();

				// report results
				double generation_best_fitness = IsMinimizing() ? cma.solution().value : -cma.solution().value;
				double current_avg_fitness = IsMinimizing() ? cma.average() : -cma.average();

				if ( GetProgressOutput() )
					printf( " A=%.3f", current_avg_fitness );

				if ( GetStatusOutput() )
					OutputStatus( "generation", flut::stringf( "%d %f %f", gen, current_avg_fitness, generation_best_fitness ) );

				bool new_best = IsBetterThan( generation_best_fitness, m_BestFitness );
				if ( new_best )
				{
					m_BestFitness = generation_best_fitness;

					if ( GetProgressOutput() )
						printf( " B=%.3f", m_BestFitness );
					if ( GetStatusOutput() )
						OutputStatus( "best", m_BestFitness );
				}

				if ( new_best || ( gen - m_LastFileOutputGen > max_generations_without_file_output ) )
				{
					// copy best solution to par
					std::vector< double > values( cma.solution().point.begin(), cma.solution().point.end() );
					par.SetFreeParamValues( values );

					// update mean / std
					std::vector< double > mean( cma.mean().begin(), cma.mean().end() );
					par.UpdateMeanStd( mean, cma.population_std() );

					// update best params after mean / std have been updated
					if ( new_best )
						m_BestParams = par;

					m_LastFileOutputGen = gen;

					// write .par file
					String ind_name = flut::stringf( "%04d_%.3f_%.3f", gen, current_avg_fitness, generation_best_fitness );
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
			}
			if ( console_output )
				std::cout << "Optimization finished" << std::endl;

			if ( GetStatusOutput() )
				OutputStatus( "finished", 1 );

		}

	}
}

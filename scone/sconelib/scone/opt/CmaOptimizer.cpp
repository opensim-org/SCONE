#include "CmaOptimizer.h"

#ifndef SHARK_VERSION
#error "No SHARK_VERSION defined"
#endif // !SHARK_VERSION

#if SHARK_VERSION == 2
#include <EALib/CMA.h>
#include <EALib/PopulationT.h>
#elif SHARK_VERSION == 3
#include "SharkHelperClasses.h"
#endif

#include <random>
#include <boost/format.hpp>

#include "scone/core/string_tools.h"
#include "scone/core/Log.h"

#include <flut/timer.hpp>

using flut::timer;
using std::cout;
using std::endl;

namespace scone
{
	namespace opt
	{
		const long DEFAULT_RANDOM_SEED = 123;

		CmaOptimizer::CmaOptimizer( const PropNode& props ) :
		Optimizer( props ),
		m_Mu( 0 ),
		m_Lambda( 0 ),
		m_Sigma( 1.0 ),
		max_generations( 10000 ),
		num_elitists( 0 ),
		max_attempts( 100 )
		{
			INIT_PROPERTY_NAMED( props, m_Lambda, "lambda", 0 );
			INIT_PROPERTY_NAMED( props, m_Mu, "mu", 0 );
			INIT_PROPERTY_NAMED( props, m_Sigma, "sigma", 1.0 );
			INIT_PROPERTY( props, max_generations, size_t( 10000 ) );
			INIT_PROPERTY( props, random_seed, DEFAULT_RANDOM_SEED );
			INIT_PROPERTY( props, global_std_factor, 0.0 );
			INIT_PROPERTY( props, global_std_offset, 0.0 );

#if SHARK_VERSION == 3
			INIT_PROPERTY( props, recombination_type, int( CMA_MT::SUPERLINEAR ) );
#endif
		}

		scone::String CmaOptimizer::GetClassSignature() const
		{
			return Optimizer::GetClassSignature() + ( random_seed != DEFAULT_RANDOM_SEED ? stringf( ".R%d", random_seed ) : ""  );
		}

		CmaOptimizer::~CmaOptimizer()
		{
		}

#if SHARK_VERSION == 3

		void CmaOptimizer::Run()
		{
			// make sure there is at least 1 objective and get info
			CreateObjectives( 1 );
			ParamSet par = GetObjective().MakeParamSet();
			size_t dim = par.GetFreeParamCount();

			SCONE_ASSERT( dim > 0 );

			// init lambda and mu
			if ( m_Lambda == 0 ) m_Lambda = shark::CMA::suggestLambda( dim );
			if ( m_Mu == 0 ) m_Mu = shark::CMA::suggestMu( m_Lambda );

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
			log::DebugF( "Starting optimization, dim=%d, lambda=%d, mu=%d", dim, m_Lambda, m_Mu );
			cma.init( objfunc, initPoint, m_Lambda, m_Mu, m_Sigma, initCovar );

			if ( status_output )
			{
				// print out some info
				cout << "folder=" << AcquireOutputFolder() << endl;
				cout << "dim=" << dim << endl;
				cout << "sigma=" << m_Sigma << endl;
				cout << "lambda=" << m_Lambda << endl;
				cout << "mu=" << m_Mu << endl;
				cout << "max_generations=" << max_generations << endl;
			}

			// optimization loop
			timer tmr;
			m_BestFitness = IsMinimizing() ? REAL_MAX : REAL_LOWEST;
			for ( size_t gen = 0; gen < max_generations; ++gen )
			{
				if ( GetProgressOutput() )
					printf("%04d:", int( gen ) ); // MSVC2013 doesn't support %zu

				cma.step_mt();

				// report results
				double generation_best_fitness = IsMinimizing() ? cma.solution().value : -cma.solution().value;
				double current_avg_fitness = IsMinimizing() ? cma.average() : -cma.average();

				if ( GetProgressOutput() )
					printf(" A=%.3f", current_avg_fitness );
				if ( status_output )
					std::cout << "generation=" << gen << " " << current_avg_fitness << " " << generation_best_fitness << std::endl;

				bool new_best = IsBetterThan( generation_best_fitness, m_BestFitness );
				if ( new_best )
				{
					m_BestFitness = generation_best_fitness;

					if ( GetProgressOutput() )
						printf(" B=%.3f", m_BestFitness );
					if ( status_output )
						std::cout << "best=" << m_BestFitness << std::endl;
				}

				if ( new_best || ( gen - m_LastFileOutputGen > max_generations_without_file_output )  )
				{
					// copy best solution to par
					std::vector< double > values( cma.solution().point.begin(), cma.solution().point.end() );
					par.SetFreeParamValues( values );

					// update mean / std
					std::vector< double > mean( dim ), std( dim );
					for ( size_t i = 0; i < dim; ++i )
					{
						mean[ i ] = cma.mean()[ i ];
						std[ i ] = sqrt( cma.covarianceMatrix()( i, i ) );
					}
					par.UpdateMeanStd( mean, std );

					// update best params after mean / std have been updated
					if ( new_best)
						m_BestParams = par;

					m_LastFileOutputGen = gen;

					// write .par file
					String ind_name = stringf( "%04d_%.3f_%.3f", gen, current_avg_fitness, generation_best_fitness );
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
				cout << "Optimization finished" << endl;

			if ( status_output )
				cout << "finished=1" << endl;
		}

#elif SHARK_VERSION == 2

		void CmaOptimizer::Run()
		{
			// make sure there is at least 1 objective and get info
			CreateObjectives( 1 );
			ParamSet par = GetObjective().MakeParamSet();
			size_t dim = par.GetFreeParamCount();

			SCONE_ASSERT( dim > 0 );

			// init lambda and mu
			if ( m_Lambda == 0 ) m_Lambda = CMA::suggestLambda( dim );
			if ( m_Mu == 0 ) m_Mu = CMA::suggestMu( m_Lambda );

			// create m_Lambda objectives
			CreateObjectives( m_Lambda );

			// init parents and offspring
			CMA cma;
			std::unique_ptr< PopulationT< double > > pParents;
			std::unique_ptr< PopulationT< double > > pOffspring;
			pParents = std::make_unique< PopulationT< double > >( m_Mu, ChromosomeT<double>( dim ), ChromosomeT<double>( dim ) );
			pOffspring = std::make_unique< PopulationT< double > >( m_Lambda, ChromosomeT<double>( dim ), ChromosomeT<double>( dim ) );

			if ( IsMinimizing() )
			{
				pParents->setMinimize();
				pOffspring->setMinimize();
			}
			else
			{
				pParents->setMaximize();
				pOffspring->setMaximize();
			}

			// init random seed
			if ( random_seed == 0 ) random_seed = long( time( NULL ) );
			Rng::seed( random_seed );

			// initialize settings from file
			if ( use_init_file && !init_file.empty() )
				par.Read( init_file );

			// generate random initial population
			for ( size_t ind_idx = 0; ind_idx < pOffspring->size(); ++ind_idx )
			{
				par.InitRandom();
				std::vector< double > vec = par.GetFreeParamValues();
				for ( size_t par_idx = 0; par_idx < dim; ++par_idx )
					(*pOffspring)[ind_idx][0][par_idx] = vec[par_idx];
			}

			// init mean and variance
			std::vector< double > avg( dim, 0.0 ), var( dim, 0.0 );
			for ( size_t par_idx = 0; par_idx < dim; ++par_idx )
			{
				for ( size_t ind_idx = 0; ind_idx < pOffspring->size(); ++ind_idx )
					avg[ par_idx ] += (*pOffspring)[ind_idx][0][par_idx];
				avg[ par_idx ] /= m_Lambda;

				for ( size_t ind_idx = 0; ind_idx < pOffspring->size(); ++ind_idx )
					var[ par_idx ] += GetSquared((*pOffspring)[ind_idx][0][par_idx] - avg[ par_idx ]);
				var[ par_idx ] /= m_Lambda;
			}

			// init CMA object
			CMA::RecombType rc_type = static_cast< CMA::RecombType >( CMA::superlinear );
			cma.init( dim, var, m_Sigma, *pParents, rc_type, CMA::rankmu );

			log::InfoF( "Starting optimization, dim=%d, lambda=%d, mu=%d", dim, m_Lambda, m_Mu );

			// optimization loop
			timer tmr;
			double best = IsMinimizing() ? REAL_MAX : REAL_LOWEST;
			for ( size_t gen = 0; gen < max_generations; ++gen )
			{
				printf("%04d:", gen );

				// setup parameter sets
				par.SetMode( ParamSet::UpdateMode );
				std::vector< ParamSet > parsets( m_Lambda, par );
				for ( size_t ind_idx = 0; ind_idx < pOffspring->size(); ++ind_idx )
					parsets[ ind_idx ].SetFreeParamValues( (*pOffspring)[ind_idx][0] );

				// evaluate parameter sets
				std::vector< double > fitnesses = Evaluate( parsets );
				for ( size_t ind_idx = 0; ind_idx < pOffspring->size(); ++ind_idx )
					(*pOffspring)[ ind_idx ].setFitness( fitnesses[ ind_idx ] );

				// report results
				printf(" M=%.3f", pOffspring->meanFitness() );
				bool new_best = IsBetterThan( pOffspring->best().fitnessValue(), best );
				if ( new_best )
				{
					best = pOffspring->best().fitnessValue();
					printf(" B=%.3f", best );

					// write results
					String ind_name = stringf( "%04d_%.3f_%.3f", gen, pOffspring->meanFitness(), best );
					String file_base = AcquireOutputFolder() + ind_name;
					parsets[ pOffspring->bestIndex() ].UpdateMeanStd( parsets );

					// keep output files
					std::vector< String > outputFiles;
					if ( output_objective_result_files )
						outputFiles = m_Objectives[ pOffspring->bestIndex() ]->WriteResults( file_base );

					// write .par file
					parsets[ pOffspring->bestIndex() ].Write( file_base + ".par" );
					outputFiles.push_back( file_base + ".par" );

					// cleanup superfluous output files
					ManageFileOutput( best, outputFiles );
				}

				// show time if needed
				if ( show_optimization_time )
					printf( " T=%.1f", tmr.GetTime() );

				// done reporting
				printf( new_best ? "\n" : "\r" );

				// update next generation
				pParents->selectMuLambda( *pOffspring, num_elitists );
				cma.updateStrategyParameters( *pParents );

				// create new offspring
				for ( size_t i = 0; i < pOffspring->size(); ++i )
				{
					for ( int attempt = 0; attempt < max_attempts; ++attempt )
					{
						cma.create( (*pOffspring)[i] );
						par.SetFreeParamValues( (*pOffspring)[i][0] );
						if ( par.CheckValues() )
							break;
					}

					if ( !par.CheckValues() )
					{
						printf("%03d: Failed to create valid individual after %d attempts, fixing instead\n", i, max_attempts);
						par.RestrainValues();
						(*pOffspring)[i][0] = par.GetFreeParamValues();
					}
				}
			}
		}

#endif // USE_SHARK_V2

	}
}

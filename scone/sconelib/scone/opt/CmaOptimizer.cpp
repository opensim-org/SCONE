#include "CmaOptimizer.h"

#if USE_SHARK_V2
#include <EALib/CMA.h>
#include <EALib/PopulationT.h>
#else
#include "SharkHelperClasses.h"
#endif

#include <random>
#include <boost/format.hpp>

#include "scone/core/tools.h"
#include "scone/core/Log.h"

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
		}

		scone::String CmaOptimizer::GetClassSignature() const
		{
			return Optimizer::GetClassSignature() + ( random_seed != DEFAULT_RANDOM_SEED ? stringf( ".R%d", random_seed ) : ""  );
		}

		CmaOptimizer::~CmaOptimizer()
		{
		}

#if USE_SHARK_V2
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

#else

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

			// init parents and offspring
			CMA_MT cma( *this );
			SconeSingleObjectiveFunction objfunc( GetObjective(), IsMinimizing() );

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
			for ( size_t par_idx = 0; par_idx < initPoint.size(); ++par_idx )
			{
				auto& parinf = par.GetParamInfo( par_idx );
				initPoint[ par_idx ] = parinf.init_mean;
				initCovar( par_idx, par_idx ) = parinf.init_std * parinf.init_std;
			}

			// init CMA object
			log::InfoF( "Starting optimization, dim=%d, lambda=%d, mu=%d", dim, m_Lambda, m_Mu );
			cma.init( objfunc, initPoint, m_Lambda, m_Mu, m_Sigma, initCovar );

			// optimization loop
			timer tmr;
			m_BestFitness = IsMinimizing() ? REAL_MAX : REAL_LOWEST;
			for ( size_t gen = 0; gen < max_generations; ++gen )
			{
				if ( console_output )
					printf("%04d:", int( gen ) ); // MSVC2013 doesn't support %zu

				cma.step_mt();

				// report results
				double current_best_fitness = IsMinimizing() ? cma.solution().value : -cma.solution().value;
				double current_avg_fitness = IsMinimizing() ? cma.average() : -cma.average();
				if ( console_output )
					printf(" A=%.3f", current_avg_fitness );

				bool new_best = IsBetterThan( current_best_fitness, m_BestFitness );
				if ( new_best )
				{
					m_BestFitness = current_best_fitness;

					if ( console_output )
						printf(" B=%.3f", m_BestFitness );

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
					m_BestParams = par;

					// write .par file
					String ind_name = stringf( "%04d_%.3f_%.3f", gen, current_avg_fitness, m_BestFitness );
					String file_base = AcquireOutputFolder() + ind_name;
					std::vector< String > outputFiles;
					par.Write( file_base + ".par" );
					outputFiles.push_back( file_base + ".par" );

					// cleanup superfluous output files
					ManageFileOutput( m_BestFitness, outputFiles );
				}

				// show time if needed
				if ( console_output )
				{
					if ( show_optimization_time )
						printf( " T=%.1f", tmr.seconds() );

					printf( new_best ? "\n" : "\r" ); // only start newline if there's been a new best
				}
			}
		}
#endif
	}
}

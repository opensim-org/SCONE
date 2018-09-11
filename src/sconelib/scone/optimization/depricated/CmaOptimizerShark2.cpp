#ifdef USE_SHARK_2

#include "CmaOptimizerShark2.h"

#include <EALib/CMA.h>
#include <EALib/PopulationT.h>

#include "flut/timer.hpp"

namespace scone
{
	CmaOptimizerShark2::CmaOptimizerShark2( const PropNode& props ) : CmaOptimizer( props )
	{
	
	}

	void CmaOptimizerShark2::Run()
	{
		// make sure there is at least 1 objective and get info
		CreateObjectives( 1 );
		ParamSet par = GetObjective().GetParamInfo();
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

		if ( status_output )
		{
			// print out some info
			cout << "folder=" << AcquireOutputFolder() << endl;
			cout << "dim=" << dim << endl;
			cout << "sigma=" << m_Sigma << endl;
			cout << "lambda=" << m_Lambda << endl;
			cout << "mu=" << m_Mu << endl;
			cout << "max_generations=" << max_generations << endl;
			cout << "SHARK_VERSION=" << SHARK_VERSION << endl;
		}

		// initialize settings from file
		if ( use_init_file && !init_file.empty() )
			par.Read( init_file );

		// generate random initial population
		for ( size_t ind_idx = 0; ind_idx < pOffspring->size(); ++ind_idx )
		{
			par.InitRandom();
			std::vector< double > vec = par.GetFreeParamValues();
			for ( size_t par_idx = 0; par_idx < dim; ++par_idx )
				( *pOffspring )[ ind_idx ][ 0 ][ par_idx ] = vec[ par_idx ];
		}

		// init mean and variance
		std::vector< double > avg( dim, 0.0 ), var( dim, 0.0 );
		for ( size_t par_idx = 0; par_idx < dim; ++par_idx )
		{
			for ( size_t ind_idx = 0; ind_idx < pOffspring->size(); ++ind_idx )
				avg[ par_idx ] += ( *pOffspring )[ ind_idx ][ 0 ][ par_idx ];
			avg[ par_idx ] /= m_Lambda;

			for ( size_t ind_idx = 0; ind_idx < pOffspring->size(); ++ind_idx )
				var[ par_idx ] += GetSquared( ( *pOffspring )[ ind_idx ][ 0 ][ par_idx ] - avg[ par_idx ] );
			var[ par_idx ] /= m_Lambda;
		}

		// init CMA object
		CMA::RecombType rc_type = static_cast<CMA::RecombType>( CMA::superlinear );
		cma.init( dim, var, m_Sigma, *pParents, rc_type, CMA::rankmu );

		log::InfoF( "Starting optimization, dim=%d, lambda=%d, mu=%d", dim, m_Lambda, m_Mu );

		// optimization loop
		flut::timer tmr;
		m_BestFitness = IsMinimizing() ? REAL_MAX : REAL_LOWEST;
		for ( size_t gen = 0; gen < max_generations; ++gen )
		{
			if ( GetProgressOutput() )
				printf( "%04d:", gen );

			// setup parameter sets
			par.SetMode( ParamSet::UpdateMode );
			std::vector< ParamSet > parsets( m_Lambda, par );
			for ( size_t ind_idx = 0; ind_idx < pOffspring->size(); ++ind_idx )
				parsets[ ind_idx ].SetFreeParamValues( ( *pOffspring )[ ind_idx ][ 0 ] );

			// evaluate parameter sets
			std::vector< double > fitnesses = Evaluate( parsets );
			for ( size_t ind_idx = 0; ind_idx < pOffspring->size(); ++ind_idx )
				( *pOffspring )[ ind_idx ].setFitness( fitnesses[ ind_idx ] );

			// report results
			if ( GetProgressOutput() )
				printf( " A=%.3f", pOffspring->meanFitness() );

			if ( status_output )
				std::cout << "generation=" << gen << " " << pOffspring->meanFitness() << " " << pOffspring->best().fitnessValue() << std::endl;

			bool new_best = IsBetterThan( pOffspring->best().fitnessValue(), m_BestFitness );
			if ( new_best )
			{
				m_BestFitness = pOffspring->best().fitnessValue();
				if ( GetProgressOutput() )
					printf( " B=%.3f", m_BestFitness );
				if ( status_output )
					std::cout << "best=" << m_BestFitness << std::endl;

				// write results
				String ind_name = stringf( "%04d_%.3f_%.3f", gen, pOffspring->meanFitness(), m_BestFitness );
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
				ManageFileOutput( m_BestFitness, outputFiles );
			}

			// show time if needed
			if ( GetProgressOutput() )
			{
				if ( show_optimization_time )
					printf( " T=%.1f", tmr.seconds() );

				// done reporting
				printf( new_best ? "\n" : "\r" );
			}

			// update next generation
			pParents->selectMuLambda( *pOffspring, num_elitists );
			cma.updateStrategyParameters( *pParents );

			// create new offspring
			for ( size_t i = 0; i < pOffspring->size(); ++i )
			{
				for ( int attempt = 0; attempt < max_attempts; ++attempt )
				{
					cma.create( ( *pOffspring )[ i ] );
					par.SetFreeParamValues( ( *pOffspring )[ i ][ 0 ] );
					if ( par.CheckValues() )
						break;
				}

				if ( !par.CheckValues() )
				{
					if ( GetProgressOutput() )
						printf( "%03d: Failed to create valid individual after %d attempts, fixing instead\n", i, max_attempts );
					par.ClampValues();
					( *pOffspring )[ i ][ 0 ] = par.GetFreeParamValues();
				}
			}
		}

		if ( status_output )
			std::cout << "finished=1" << std::endl;
	}
}

#endif
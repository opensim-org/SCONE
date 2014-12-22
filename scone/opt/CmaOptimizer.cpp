#include "stdafx.h"
#include "CmaOptimizer.h"

#include <EALib/CMA.h>
#include <EALib/PopulationT.h>

#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include "../core/Timer.h"

namespace scone
{
	namespace opt
	{
		typedef PopulationT< double > Population;
		typedef std::unique_ptr< Population > PopulationPtr;
		struct CmaOptimizer::Impl
		{
			CMA m_CMA;

			PopulationPtr m_pParents;
			PopulationPtr m_pOffspring;

			Population& Parents() { return *m_pParents; }
			Population& Offspring() { return *m_pOffspring; }
		};


		CmaOptimizer::CmaOptimizer() :
		m_pImpl( new Impl ),
		m_Mu( 0 ),
		m_Lambda( 0 ),
		m_Sigma( 1.0 ),
		max_generations( 10000 ),
		num_elitists( 0 ),
		max_attempts( 100 )
		{
		}

		CmaOptimizer::~CmaOptimizer()
		{
		}

		void CmaOptimizer::ProcessProperties( const PropNode& props )
		{
			Optimizer::ProcessProperties( props );

			INIT_FROM_PROP_NAMED( props, m_Lambda, "lamda", 0 );
			INIT_FROM_PROP_NAMED( props, m_Mu, "mu", 0 );
			INIT_FROM_PROP_NAMED( props, m_Sigma, "sigma", 1.0 );
			INIT_FROM_PROP( props, max_generations, 10000u );
		}

		void CmaOptimizer::Run()
		{
			// get info from objective
			ParamSet par = GetObjective()->GetParamSet();
			size_t dim = par.GetFreeParamCount();

			SCONE_ASSERT( dim > 0 );

			// init lambda and mu
			if ( m_Lambda == 0 ) m_Lambda = CMA::suggestLambda( dim );
			if ( m_Mu == 0 ) m_Mu = CMA::suggestMu( m_Lambda );

			// init parents and offspring
			m_pImpl->m_pParents = PopulationPtr( new Population( m_Mu, ChromosomeT<double>( dim ), ChromosomeT<double>( dim ) ) );
			m_pImpl->m_pOffspring = PopulationPtr( new Population( m_Lambda, ChromosomeT<double>( dim ), ChromosomeT<double>( dim ) ) );
			m_pImpl->m_pParents->setMinimize();
			m_pImpl->m_pOffspring->setMinimize();

			// init random seed
			Rng::seed( 123 );

			// generate random population
			for ( size_t ind_idx = 0; ind_idx < m_pImpl->m_pOffspring->size(); ++ind_idx )
			{
				par.InitRandom();
				std::vector< double > vec = par.GetFreeParamValues();
				for ( size_t par_idx = 0; par_idx < dim; ++par_idx )
					(*m_pImpl->m_pOffspring)[ind_idx][0][par_idx] = vec[par_idx];
			}

			// init mean and variance
			std::vector< double > avg( dim, 0.0 ), var( dim, 0.0 );
			for ( size_t par_idx = 0; par_idx < dim; ++par_idx )
			{
				for ( size_t ind_idx = 0; ind_idx < m_pImpl->m_pOffspring->size(); ++ind_idx )
					avg[ par_idx ] += (*m_pImpl->m_pOffspring)[ind_idx][0][par_idx];
				avg[ par_idx ] /= m_Lambda;

				for ( size_t ind_idx = 0; ind_idx < m_pImpl->m_pOffspring->size(); ++ind_idx )
					var[ par_idx ] += Square((*m_pImpl->m_pOffspring)[ind_idx][0][par_idx] - avg[ par_idx ]);
				var[ par_idx ] /= m_Lambda;
			}

			// init CMA object
			CMA::RecombType rc_type = static_cast< CMA::RecombType >( CMA::superlinear );
			m_pImpl->m_CMA.init( dim, var, m_Sigma, *m_pImpl->m_pParents, rc_type, CMA::rankmu );

			// optimization loop
			Timer timer;
			double best = REAL_MAX;
			for ( size_t gen = 0; gen < max_generations; ++gen )
			{
				printf("%04d:", gen );

				// setup parameter sets
				std::vector< ParamSet > parsets( m_Lambda, par );
				for ( size_t ind_idx = 0; ind_idx < m_pImpl->m_pOffspring->size(); ++ind_idx )
					parsets[ ind_idx ].SetFreeParamValues( (*m_pImpl->m_pOffspring)[ind_idx][0] );

				// evaluate parameter sets
				std::vector< double > fitnesses = Evaluate( parsets );
				for ( size_t ind_idx = 0; ind_idx < m_pImpl->m_pOffspring->size(); ++ind_idx )
					(*m_pImpl->m_pOffspring)[ ind_idx ].setFitness( fitnesses[ ind_idx ] );

				// report results
				printf(" M=%5.2f", (*m_pImpl->m_pOffspring).meanFitness() );
				if ( (*m_pImpl->m_pOffspring).best().fitnessValue() < best )
				{
					best = (*m_pImpl->m_pOffspring).best().fitnessValue();
					printf(" B=%5.2f", best );
				}

				if ( (gen + 1) % 5 == 0 ) printf( " T=%.2f", timer.GetTime() );

				// update next generation
				m_pImpl->m_pParents->selectMuLambda( *m_pImpl->m_pOffspring, num_elitists );
				m_pImpl->m_CMA.updateStrategyParameters( *m_pImpl->m_pParents );

				// create new offspring
				for ( size_t i = 0; i < m_pImpl->m_pOffspring->size(); ++i )
				{
					for ( int attempt = 0; attempt < max_attempts; ++attempt )
					{
						m_pImpl->m_CMA.create( ( *m_pImpl->m_pOffspring )[i] );
						par.SetFreeParamValues( ( *m_pImpl->m_pOffspring )[i][0] );
						if ( par.CheckValues() )
							break;
					}

					if ( !par.CheckValues() )
					{
						printf("%03d: Failed to create valid individual after %d attempts, fixing instead\n", i, max_attempts);
						par.RestrainValues();
						m_pImpl->Offspring()[i][0] = par.GetFreeParamValues();
					}
				}
				printf( "\n" );
			}
		}
	}
}

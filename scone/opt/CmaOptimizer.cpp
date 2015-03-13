#include "stdafx.h"
#include "CmaOptimizer.h"

#include <EALib/CMA.h>
#include <EALib/PopulationT.h>

#include <boost/format.hpp>
#include <boost/foreach.hpp>

#include "../core/Timer.h"
#include "../core/Log.h"

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


		CmaOptimizer::CmaOptimizer( const PropNode& props ) :
		Optimizer( props ),
		m_pImpl( new Impl ),
		m_Mu( 0 ),
		m_Lambda( 0 ),
		m_Sigma( 1.0 ),
		max_generations( 10000 ),
		num_elitists( 0 ),
		max_attempts( 100 )
		{
			INIT_FROM_PROP_NAMED( props, m_Lambda, "lambda", 0 );
			INIT_FROM_PROP_NAMED( props, m_Mu, "mu", 0 );
			INIT_FROM_PROP_NAMED( props, m_Sigma, "sigma", 1.0 );
			INIT_FROM_PROP( props, max_generations, 10000u );
			INIT_FROM_PROP( props, random_seed, long( 123 ) );
		}

		CmaOptimizer::~CmaOptimizer()
		{
		}

		void CmaOptimizer::Run()
		{
			// make sure there is at least 1 objective and get info
			CreateObjectives( 1 );
			ParamSet par = GetObjective().GetParamSet();
			size_t dim = par.GetFreeParamCount();

			SCONE_ASSERT( dim > 0 );

			// init lambda and mu
			if ( m_Lambda == 0 ) m_Lambda = CMA::suggestLambda( dim );
			if ( m_Mu == 0 ) m_Mu = CMA::suggestMu( m_Lambda );

			// create m_Lambda objectives
			CreateObjectives( m_Lambda );

			// init parents and offspring
			m_pImpl->m_pParents = PopulationPtr( new Population( m_Mu, ChromosomeT<double>( dim ), ChromosomeT<double>( dim ) ) );
			m_pImpl->m_pOffspring = PopulationPtr( new Population( m_Lambda, ChromosomeT<double>( dim ), ChromosomeT<double>( dim ) ) );

			if ( IsMinimizing() )
			{
				m_pImpl->m_pParents->setMinimize();
				m_pImpl->m_pOffspring->setMinimize();
			}
			else
			{
				m_pImpl->m_pParents->setMaximize();
				m_pImpl->m_pOffspring->setMaximize();
			}

			// init random seed
			Rng::seed( random_seed );

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

			SCONE_LOG( "Starting optimization, dim=" << dim << " lambda=" << m_Lambda << " mu=" << m_Mu );

			// optimization loop
			Timer timer;
			double best = IsMinimizing() ? REAL_MAX : REAL_LOWEST;
			for ( size_t gen = 0; gen < max_generations; ++gen )
			{
				printf("%04d:", gen );

				// setup parameter sets
				par.SetMode( ParamSet::UpdateMode );
				std::vector< ParamSet > parsets( m_Lambda, par );
				for ( size_t ind_idx = 0; ind_idx < m_pImpl->m_pOffspring->size(); ++ind_idx )
					parsets[ ind_idx ].SetFreeParamValues( (*m_pImpl->m_pOffspring)[ind_idx][0] );

				// evaluate parameter sets
				std::vector< double > fitnesses = Evaluate( parsets );
				for ( size_t ind_idx = 0; ind_idx < m_pImpl->m_pOffspring->size(); ++ind_idx )
					(*m_pImpl->m_pOffspring)[ ind_idx ].setFitness( fitnesses[ ind_idx ] );

				// report results
				printf(" M=%.2f", m_pImpl->Offspring().meanFitness() );
				if ( IsBetterThan( m_pImpl->Offspring().best().fitnessValue(), best ) )
				{
					best = m_pImpl->Offspring().best().fitnessValue();
					printf(" B=%.2f", best );

					// write results
					String ind_name = GetStringF( "%04d_%.3f_%.3f", gen, m_pImpl->Offspring().meanFitness(), best );
					String file_base = GetOutputFolder() + ind_name;
					parsets[ m_pImpl->Offspring().bestIndex() ].UpdateMeanStd( parsets );
					parsets[ m_pImpl->Offspring().bestIndex() ].Write( file_base + ".par" );
					m_Objectives[ m_pImpl->Offspring().bestIndex() ]->WriteResults( file_base );
				}

				// show time if needed
				if ( show_optimization_time )
					printf( " T=%.1f", timer.GetTime() );

				// done reporting
				printf( "\n" );

				// update next generation
				m_pImpl->m_pParents->selectMuLambda( m_pImpl->Offspring(), num_elitists );
				m_pImpl->m_CMA.updateStrategyParameters( m_pImpl->Parents() );

				// create new offspring
				for ( size_t i = 0; i < m_pImpl->m_pOffspring->size(); ++i )
				{
					for ( int attempt = 0; attempt < max_attempts; ++attempt )
					{
						m_pImpl->m_CMA.create( m_pImpl->Offspring()[i] );
						par.SetFreeParamValues( m_pImpl->Offspring()[i][0] );
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
			}
		}
	}
}

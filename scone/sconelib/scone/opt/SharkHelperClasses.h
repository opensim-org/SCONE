#pragma once

// define these variables because shark includes windows.h
#if defined(_MSC_VER)
#	define NOMINMAX
#	define WIN32_LEAN_AND_MEAN
#endif

const int MAX_INDIVIDUAL_SAMPLE_ATTEMPTS = 256;

#include <shark/Algorithms/DirectSearch/CMA.h>
#include <shark/Algorithms/DirectSearch/Operators/Evaluation/PenalizingEvaluator.h>
#include <shark/Algorithms/DirectSearch/FitnessExtractor.h>
#include <shark/Algorithms/DirectSearch/Operators/Selection/ElitistSelection.h>

#include "Objective.h"

namespace scone
{
	namespace opt
	{
		class SconeSingleObjectiveFunction : public shark::SingleObjectiveFunction
		{
		public:
			SconeSingleObjectiveFunction( Objective& o, bool minimize ) : m_Objective( o ), m_Minimize( minimize ) {}
			virtual std::size_t numberOfVariables() const override { return m_Objective.MakeParamSet().GetFreeParamCount(); }
			virtual ResultType eval( const SearchPointType& input ) const override
			{
				std::vector< double > v( input.begin(), input.end() );
				auto par = m_Objective.MakeParamSet();
				par.SetFreeParamValues( v );
				auto result = m_Objective.Evaluate( par );
				return m_Minimize ? result : -result;
			}

		private:
			Objective& m_Objective;
			bool m_Minimize;
		};

		void EvaluateSharkIndividuals( Optimizer& opt, std::vector< shark::Individual<shark::RealVector, double, shark::RealVector> >& individuals )
		{
			// setup parameter sets
			auto par = opt.GetObjective().MakeParamSet();
			par.SetMode( ParamSet::UpdateMode );
			std::vector< ParamSet > parsets( individuals.size(), par );
			for ( size_t ind_idx = 0; ind_idx < individuals.size(); ++ind_idx )
			{
				std::vector< double > v( individuals[ ind_idx ].searchPoint().begin(), individuals[ ind_idx ].searchPoint().end() );
				parsets[ind_idx].SetFreeParamValues( v );
				SCONE_ASSERT( parsets[ind_idx].CheckValues() ); // values must have been checked at this point
			}

			// evaluate parameter sets using scone::Optimizer
			std::vector< double > fitnesses = opt.Evaluate( parsets );

			// copy back results
			double factor = opt.IsMinimizing() ? 1.0 : -1.0;
			for ( size_t ind_idx = 0; ind_idx < individuals.size(); ++ind_idx )
			{
				individuals[ ind_idx ].unpenalizedFitness() = factor * fitnesses[ ind_idx ];
				individuals[ ind_idx ].penalizedFitness() = factor * fitnesses[ ind_idx ];
			}
		}

		class CMA_MT : public shark::CMA
		{
		public:
			CMA_MT( Optimizer& opt ) : CMA(), m_Optimizer( opt ), m_Average( 0 ) {}

			double average() { return m_Average; }
			void step_mt()
			{
				std::vector< shark::Individual<shark::RealVector, double, shark::RealVector> > offspring( m_lambda );

				// get ParamSet instance for checking and clamping parameter boundaries.
				auto par = m_Optimizer.GetObjective().MakeParamSet();
				par.SetMode( ParamSet::UpdateMode );
				SCONE_ASSERT( par.GetFreeParamCount() == m_numberOfVariables );

				for( std::size_t i = 0; i < offspring.size(); i++ )
				{
					auto& off = offspring[i];
					bool is_within_range = false;
					for ( int attempt = 0; attempt < MAX_INDIVIDUAL_SAMPLE_ATTEMPTS && !is_within_range; ++attempt )
					{
						shark::MultiVariateNormalDistribution::result_type sample = m_mutationDistribution();
						off.chromosome() = sample.second;
						off.searchPoint() = m_mean + m_sigma * sample.first;
						is_within_range = par.CheckFreeParamValues( off.searchPoint().begin(), off.searchPoint().end() );
					}

					if ( !is_within_range ) // failed to find proper sample, clamp instead
					{
						log::warning( "Could not find individual after 3 attempts, clamping parameter values" );
						par.ClampFreeParamValues( off.searchPoint().begin(), off.searchPoint().end() );
					}
				}

				EvaluateSharkIndividuals( m_Optimizer, offspring );
				
				// compute average
				m_Average = 0;			
				for ( auto ind : offspring )
					m_Average += ind.unpenalizedFitness() / offspring.size();

				// Selection
				std::vector< shark::Individual<shark::RealVector, double, shark::RealVector> > parents( m_mu );
				shark::ElitistSelection<shark::FitnessExtractor> selection;
				selection(offspring.begin(),offspring.end(),parents.begin(), parents.end());
				// Strategy parameter update
				m_counter++; // increase generation counter
				updateStrategyParameters( parents );

				m_best.point= parents[ 0 ].searchPoint();
				m_best.value= parents[ 0 ].unpenalizedFitness();
			}

		private:
			Optimizer& m_Optimizer;
			double m_Average;
		};
	}
}

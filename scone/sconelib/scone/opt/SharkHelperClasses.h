#pragma once

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
				parsets[ind_idx].RestrainValues(); // ensure the values are within min / max

				// important: copy restrained values back to Shark individuals, as they are used to store results
				v = parsets[ ind_idx ].GetFreeParamValues();
				std::copy( v.begin(), v.end(), individuals[ ind_idx ].searchPoint().begin() ); 
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

				for( std::size_t i = 0; i < offspring.size(); i++ ) {
					shark::MultiVariateNormalDistribution::result_type sample = m_mutationDistribution();
					offspring[i].chromosome() = sample.second;
					offspring[i].searchPoint() = m_mean + m_sigma * sample.first;
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

#pragma once

#include "opt.h"
#include "Objective.h"
#include "..\core\Propertyable.h"

namespace scone
{
	namespace opt
	{
		class OPT_API Optimizer : public Propertyable
		{
		public:
			Optimizer();
			virtual ~Optimizer();

			virtual void ProcessProperties( const PropNode& props ) override;
			ObjectiveSP GetObjective() { SCONE_ASSERT( m_Objectives.size() > 0 ); return m_Objectives[ 0 ]; }
			virtual void Run() = 0;

		protected:
			std::vector< double > Evaluate( std::vector< ParamSet >& parsets );

		private:
			std::vector< double > EvaluateSingleThreaded( std::vector< ParamSet >& parsets );
			std::vector< double > EvaluateMultiThreaded( std::vector< ParamSet >& parsets );
			static void Optimizer::EvaluateFunc( ObjectiveSP obj, ParamSet& par, double* fitness, int priority );

			size_t max_threads;
			int thread_priority;
			std::vector< std::shared_ptr< Objective > > m_Objectives;
		};
	}
}

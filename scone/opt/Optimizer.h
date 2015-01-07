#pragma once

#include "opt.h"
#include "Objective.h"
#include "../core/Propertyable.h"

namespace scone
{
	namespace opt
	{
		class OPT_API Optimizer : public Propertyable
		{
		public:
			Optimizer( const PropNode& props );
			virtual ~Optimizer();

			Objective& GetObjective() { SCONE_ASSERT( m_Objectives.size() > 0 ); return *m_Objectives[ 0 ]; }
			virtual void Run() = 0;

			/// get the results output folder (creates it if it doesn't exist)
			const String& GetOutputFolder();

		protected:
			std::vector< double > Evaluate( std::vector< ParamSet >& parsets );
			PropNode m_ObjectiveProps;
			std::vector< ObjectiveUP > m_Objectives;

		private:
			std::vector< double > EvaluateSingleThreaded( std::vector< ParamSet >& parsets );
			std::vector< double > EvaluateMultiThreaded( std::vector< ParamSet >& parsets );
			static void Optimizer::EvaluateFunc( Objective* obj, ParamSet& par, double* fitness, int priority );
			void InitOutputFolder();

			size_t max_threads;
			int thread_priority;
			String output_folder_base;
			String m_Name;

			String m_OutputFolder;

		private: // non-copyable and non-assignable
			Optimizer( const Optimizer& );
			Optimizer& operator=( const Optimizer& );
		};
	}
}

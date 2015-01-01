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
			Objective& GetObjective() { SCONE_ASSERT( m_Objectives.size() > 0 ); return *m_Objectives[ 0 ]; }
			virtual void Run() = 0;

			const String& GetOutputFolder() { return m_OutputFolder; }

		protected:
			void InitOutputFolder();
			std::vector< double > Evaluate( std::vector< ParamSet >& parsets );
			PropNode m_ObjectiveProps;
			std::vector< ObjectiveUP > m_Objectives;

		private:
			std::vector< double > EvaluateSingleThreaded( std::vector< ParamSet >& parsets );
			std::vector< double > EvaluateMultiThreaded( std::vector< ParamSet >& parsets );
			static void Optimizer::EvaluateFunc( Objective* obj, ParamSet& par, double* fitness, int priority );

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

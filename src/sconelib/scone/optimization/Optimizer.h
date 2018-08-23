#pragma once

#include "Objective.h"
#include "scone/core/HasSignature.h"
#include "scone/core/types.h"
#include "Params.h"
#include "xo/serialization/prop_node_serializer_zml.h"
#include "xo/system/error_code.h"
#include <iostream>

namespace scone
{
	class SCONE_API Optimizer : public HasSignature
	{
	public:
		Optimizer( const PropNode& props );
		virtual ~Optimizer();

		Objective& GetObjective() { return *m_Objective; }
		const Objective& GetObjective() const { return *m_Objective; }
		virtual void Run() = 0;

		/// get the results output folder (creates it if it doesn't exist)
		const path& AcquireOutputFolder() const;

		bool IsBetterThan( double v1, double v2 ) { return IsMinimizing() ? v1 < v2 : v1 > v2; }
		virtual bool IsMinimizing() const { return m_Objective->info().minimize(); }

		double GetBestFitness() { return m_BestFitness; }

		void SetConsoleOutput( bool output ) { console_output = output; }
		bool GetProgressOutput() const { return console_output && !status_output; }
		bool GetStatusOutput() const { return status_output; }
		void SetStatusOutput( bool s ) { status_output = s; }

		template< typename T > void OutputStatus( const String& key, const T& value ) const {
			std::cout << std::endl << "*" << key << "=" << value << std::endl;
		}

		void SetStatus( PropNode& pn ) const {
			pn[ "id" ] = m_Name;
			xo::error_code ec;
			std::cout << "*" << xo::prop_node_serializer_zml_concise( pn, &ec ) << std::endl;
		}

		path output_root;

		// properties
		size_t max_threads;
		int thread_priority;
		bool show_optimization_time;

		// init file
		path init_file;
		bool use_init_file;
		bool use_init_file_std;
		double init_file_std_factor;
		double init_file_std_offset;

		// file output
		Real min_improvement_factor_for_file_output;
		size_t max_generations_without_file_output;
		bool output_objective_result_files;
		mutable size_t m_LastFileOutputGen;

		// stop conditions
		size_t max_generations;
		double min_progress;
		size_t min_progress_samples;

		void ManageFileOutput( double fitness, const std::vector< path >& files ) const;

	protected:
		const PropNode& m_ObjectiveProps;
		ObjectiveUP m_Objective;
		virtual String GetClassSignature() const override;

		// current status
		double m_BestFitness;
		bool console_output;
		bool status_output;

	private:
		void InitOutputFolder() const;
		static void SetThreadPriority( int priority );

		String m_Name;
		mutable path m_OutputFolder;
		mutable std::vector< std::pair< double, std::vector< path > > > m_OutputFiles;

	private: // non-copyable and non-assignable
		Optimizer( const Optimizer& ) = delete;
		Optimizer& operator=( const Optimizer& ) = delete;
	};
}

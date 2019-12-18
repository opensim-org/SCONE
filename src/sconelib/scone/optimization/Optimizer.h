/*
** Optimizer.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "Objective.h"
#include "Params.h"
#include "scone/core/HasSignature.h"
#include "scone/core/types.h"
#include "xo/system/log_sink.h"

namespace scone
{
	/// Base class for Optimizers.
	class SCONE_API Optimizer : public HasSignature
	{
	public:
		Optimizer( const PropNode& props, const PropNode& scenario_pn, const path& scenario_dir );
		Optimizer( const Optimizer& ) = delete;
		Optimizer& operator=( const Optimizer& ) = delete;

		virtual ~Optimizer();

		/// Parameter file (.par) to be used for initial parameter values.
		path init_file;

		/// Use init_file (if exists); default = true.
		bool use_init_file;

		/// Use the standard deviations from the init_file; when set to false, the initial standard deviation is 
		/// computed as follows: STDEV = parameter value * init_file_std_factor + init_file_std_offset; default = true.
		bool use_init_file_std;

		/// Factor by which to multiply the standard deviations from the init_file; default = 1.0.
		double init_file_std_factor;

		/// Offset added to the standard deviations from init_file; default = 0.
		double init_file_std_offset;

		/// Maximum number of threads to use for this optimization; default = 32.
		size_t max_threads;

		/// Thread priority of the optimization; 0 = lowest, 7 = highest, default = 1.
		int thread_priority;

		/// Number of iterations after which to stop the optimization; default = 10000.
		size_t max_generations;

		/// Minimum progress after which to stop the optimization; default = 1e-6.
		double min_progress;

		/// Minimum number of samples after which progress is measured; default = 200.
		size_t min_progress_samples;

		/// The minimum relative improvement needed for file output; default = 0.05.
		Real min_improvement_for_file_output;

		/// The maximum number of iterations without file output; default = 1000.
		size_t max_generations_without_file_output;

		Objective& GetObjective() { return *m_Objective; }
		const Objective& GetObjective() const { return *m_Objective; }
		virtual void Run() = 0;

		// get the results output folder (creates it if it doesn't exist)
		const path& GetOutputFolder() const;

		virtual bool IsMinimizing() const { return m_Objective->info().minimize(); }

		virtual double GetBestFitness() const = 0;

		enum OutputMode { no_output, console_output, status_output };
		virtual void SetOutputMode( OutputMode m ) { output_mode_ = m; }
		bool GetProgressOutput() const { return output_mode_ == console_output; }
		bool GetStatusOutput() const { return output_mode_ == status_output; }

		PropNode GetStatusPropNode() const;
		void OutputStatus( const PropNode& pn ) const;
		template< typename T > void OutputStatus( const String& key, const T& value ) const;

		const String& id() const { return id_; }

		mutable size_t m_LastFileOutputGen;
		path output_root;
		bool show_optimization_time;
		bool output_objective_result_files;

		void PrepareOutputFolder();

	protected:
		ObjectiveUP m_Objective;
		virtual String GetClassSignature() const override;

		// current status
		double m_BestFitness;
		OutputMode output_mode_;

		mutable path output_folder_;
		mutable String id_;

		xo::log::level log_level_;
		u_ptr< xo::log::file_sink > log_sink_;

		PropNode scenario_pn_copy_; // copy for creating props in output folder
	};

	template< typename T >
	void scone::Optimizer::OutputStatus( const String& key, const T& value ) const
	{
		PropNode pn = GetStatusPropNode();
		pn.set( key, value );
		OutputStatus( pn );
	}

}

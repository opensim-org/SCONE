/*
** Optimizer.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "Optimizer.h"

#include "scone/core/Log.h"
#include "scone/core/system_tools.h"
#include "scone/core/string_tools.h"
#include "scone/core/Factories.h"
#include "scone/core/math.h"
#include "scone/optimization/Objective.h"
#include "xo/filesystem/filesystem.h"
#include "xo/container/prop_node_tools.h"

#if defined(_MSC_VER)
#	define NOMINMAX
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#endif
#include "xo/system/system_tools.h"

namespace scone
{
	Optimizer::Optimizer( const PropNode& props, const PropNode& root ) :
	HasSignature( props ),
	max_threads( 1 ),
	thread_priority( (int)xo::thread_priority::lowest ),
	m_ObjectiveProps( FindFactoryProps( GetObjectiveFactory(), props, "Objective" ) ),
	m_Objective( CreateObjective( m_ObjectiveProps ) ),
	m_BestFitness( m_Objective->info().worst_fitness() ),
	output_mode_( no_output ),
	m_LastFileOutputGen( 0 ),
	config_copy_( root )
	{
		INIT_PROP( props, output_root, GetFolder( SCONE_RESULTS_FOLDER ) );
		log_level_ = static_cast<xo::log::level>( props.get<int>( "log_level" ) );

		INIT_PROP( props, max_threads, size_t( 32 ) );
		INIT_PROP( props, thread_priority, (int)xo::thread_priority::lowest );
		INIT_PROP( props, show_optimization_time, false );

		INIT_PROP( props, init_file, path( "" ) );
		INIT_PROP( props, use_init_file, true );
		INIT_PROP( props, init_file_std_factor, 1.0 );
		INIT_PROP( props, init_file_std_offset, 0.0 );
		INIT_PROP( props, use_init_file_std, true );

		INIT_PROP( props, output_objective_result_files, false );
		INIT_PROP( props, min_improvement_for_file_output, 0.05 );
		INIT_PROP( props, max_generations_without_file_output, 1000 );

		INIT_PROP( props, max_generations, 10000 );
		INIT_PROP( props, min_progress, 1e-6 );
		INIT_PROP( props, min_progress_samples, 200 );

		// initialize parameters from file
		if ( use_init_file && !init_file.empty() )
		{
			auto result = GetObjective().info().import_mean_std( init_file, use_init_file_std, init_file_std_factor, init_file_std_offset );
			log::info( "Imported ", result.first, " of ", GetObjective().info().dim(), ", skipped ", result.second, " parameters from ", init_file );
		}
	}

	Optimizer::~Optimizer()
	{}

	const path& Optimizer::GetOutputFolder() const
	{
		SCONE_ASSERT( !output_folder_.empty() );
		return output_folder_;
	}

	PropNode Optimizer::GetStatusPropNode() const
	{
		PropNode pn;
		if ( !id_.empty() ) pn[ "id" ] = id_;
		return pn;
	}

	void Optimizer::OutputStatus( const PropNode& pn ) const
	{
		xo::error_code ec;
		std::cout << "*" << xo::prop_node_serializer_zml_concise( pn, &ec ) << std::endl;
	}

	scone::String Optimizer::GetClassSignature() const
	{
		String s = GetObjective().GetSignature();
		if ( use_init_file && !init_file.empty() )
			s += ".I";

		return s;
	}

	void Optimizer::PrepareOutputFolder()
	{
		SCONE_ASSERT( output_folder_.empty() );

		output_folder_ = xo::create_unique_folder( output_root / GetSignature() );
		id_ = output_folder_.filename().str();

		// create log sink if enabled
		if ( log_level_ < xo::log::never_log_level )
			log_sink_ = std::make_unique< xo::log::file_sink >( log_level_, GetOutputFolder() / "optimization.log" );

		// prepare output folder, and initialize
		auto outdir = GetOutputFolder();
		xo::save_file( config_copy_, GetOutputFolder() / "config.scone" );
		if ( use_init_file && !init_file.empty() )
			xo::copy_file( init_file, GetOutputFolder() / init_file.filename(), true );

		// copy all objective resources to output folder
		for ( auto& f : GetObjective().GetExternalResources() )
			xo::copy_file( f, outdir / f.filename(), true );
	}
}

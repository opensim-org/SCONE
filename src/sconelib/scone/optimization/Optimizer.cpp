#include "Optimizer.h"

#include "scone/core/Log.h"
#include "scone/core/propnode_tools.h"
#include "scone/core/system_tools.h"
#include "scone/core/string_tools.h"
#include "scone/core/Factories.h"
#include "scone/core/math.h"
#include "scone/optimization/Objective.h"

#include "xo/filesystem/filesystem.h"

#if defined(_MSC_VER)
#	define NOMINMAX
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#endif
#include "xo/system/system_tools.h"

namespace scone
{
	Optimizer::Optimizer( const PropNode& props ) :
	HasSignature( props ),
	max_threads( 1 ),
	thread_priority( (int)xo::thread_priority::lowest ),
	m_ObjectiveProps( props.get_child( "Objective" ) ),
	console_output( true ),
	status_output( false ),
	m_LastFileOutputGen( 0 ),
	output_root( GetFolder( SCONE_RESULTS_FOLDER ) )
	{
		INIT_PROPERTY_NAMED( props, m_Name, "name", String() );

		INIT_PROPERTY( props, max_threads, size_t( 32 ) );
		INIT_PROPERTY( props, thread_priority, (int)xo::thread_priority::lowest );
		INIT_PROPERTY( props, show_optimization_time, false );

		INIT_PROPERTY( props, init_file, path( "" ) );
		INIT_PROPERTY( props, use_init_file, true );
		INIT_PROPERTY( props, init_file_std_factor, 1.0 );
		INIT_PROPERTY( props, init_file_std_offset, 0.0 );
		INIT_PROPERTY( props, use_init_file_std, true );

		INIT_PROPERTY( props, output_objective_result_files, false );
		INIT_PROPERTY( props, min_improvement_factor_for_file_output, 1.05 );
		INIT_PROPERTY( props, max_generations_without_file_output, size_t( 500u ) );

		// create objective
		m_Objective = CreateObjective( m_ObjectiveProps );

		//// initialize parameters from file
		//if ( use_init_file && !init_file.empty() )
		//{
		//	auto result = GetObjective().info().import_mean_std( init_file, use_init_file_std, init_file_std_factor, init_file_std_offset );
		//	log::info( "Imported ", result.first, ", skipped ", result.second, " parameters from ", init_file );
		//}

		m_BestFitness = m_Objective->info().worst_fitness();
	}

	Optimizer::~Optimizer()
	{}

	void Optimizer::InitOutputFolder() const
	{
		auto output_base = output_root / GetSignature();
		m_OutputFolder = output_base;

		for ( int i = 1; xo::exists( xo::path( m_OutputFolder.str() ) ); ++i )
			m_OutputFolder = output_base + stringf( " (%d)", i );

		xo::create_directories( xo::path( m_OutputFolder.str() ) );
	}

	const path& Optimizer::AcquireOutputFolder() const
	{
		if ( m_OutputFolder.empty() )
			InitOutputFolder();
		return m_OutputFolder;
	}

	scone::String Optimizer::GetClassSignature() const
	{
		String s = GetObjective().GetSignature();
		if ( use_init_file && !init_file.empty() )
			s += ".I";

		return s;
	}

	void Optimizer::ManageFileOutput( double fitness, const std::vector< path >& files ) const
	{
		m_OutputFiles.push_back( std::make_pair( fitness, files ) );
		if ( m_OutputFiles.size() >= 3 )
		{
			// see if we should delete the second last file
			auto testIt = m_OutputFiles.end() - 2;
			double imp1 = testIt->first / ( testIt - 1 )->first;
			double imp2 = ( testIt + 1 )->first / testIt->first;
			if ( IsMinimizing() ) { imp1 = 1.0 / imp1; imp2 = 1.0 / imp2; }

			if ( imp1 < min_improvement_factor_for_file_output && imp2 < min_improvement_factor_for_file_output )
			{
				// delete the file(s)
				bool ok = true;
				for ( auto& file : testIt->second )
					ok &= xo::remove( file );

				if ( ok )
					m_OutputFiles.erase( testIt );
			}
		}
	}

	void Optimizer::SetThreadPriority( int priority )
	{
#ifdef _MSC_VER
		::SetThreadPriority( ::GetCurrentThread(), priority );
#elif __APPLE__
		// TODO setschedprio unavailable; maybe use getschedparam?
#else
		pthread_setschedprio( pthread_self(), priority );
#endif
	}
}

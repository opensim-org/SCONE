#include "Optimizer.h"

#include "scone/core/Log.h"
#include "scone/core/propnode_tools.h"
#include "scone/core/system_tools.h"
#include "scone/core/string_tools.h"
#include "scone/core/Factories.h"
#include "scone/core/math.h"

#include "scone/optimization/Objective.h"

#if defined(_MSC_VER)
#	define NOMINMAX
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#endif
#include "flut/filesystem.hpp"

namespace scone
{
	Optimizer::Optimizer( const PropNode& props ) :
	HasSignature( props ),
	max_threads( 1 ),
	thread_priority( 0 ),
	m_ObjectiveProps( props.get_child( "Objective" ) ),
	console_output( true ),
	status_output( false ),
	m_LastFileOutputGen( 0 ),
	output_root( GetFolder( SCONE_RESULTS_FOLDER ) )
	{
		INIT_PROPERTY( props, max_threads, size_t( 32 ) );
		INIT_PROPERTY( props, thread_priority, -2 );
		INIT_PROPERTY_NAMED( props, m_Name, "name", String() );
		INIT_PROPERTY( props, maximize_objective, false );
		INIT_PROPERTY( props, show_optimization_time, false );
		INIT_PROPERTY( props, min_improvement_factor_for_file_output, 1.05 );
		INIT_PROPERTY( props, max_generations_without_file_output, size_t( 500u ) );
		INIT_PROPERTY( props, init_file, path( "" ) );
		INIT_PROPERTY( props, use_init_file, true );
		INIT_PROPERTY( props, output_objective_result_files, false );

		m_BestFitness = maximize_objective ? REAL_LOWEST : REAL_MAX;

		// create at least one objective from props, so that all nodes are properly flagged
		CreateObjectives( 1 );
	}

	Optimizer::~Optimizer()
	{}

	void Optimizer::InitOutputFolder()
	{
		auto output_base = output_root / GetSignature();
		m_OutputFolder = output_base;

		for ( int i = 1; flut::exists( flut::path( m_OutputFolder.str() ) ); ++i )
			m_OutputFolder = output_base + stringf( " (%d)", i );

		flut::create_directories( flut::path( m_OutputFolder.str() ) );
		m_OutputFolder;
	}

	const path& Optimizer::AcquireOutputFolder()
	{
		if ( m_OutputFolder.empty() )
			InitOutputFolder();
		return m_OutputFolder;
	}

	scone::String Optimizer::GetClassSignature() const
	{
		String s = GetSconeBuildNumber() + "." + GetObjective().GetSignature();
		if ( use_init_file && !init_file.empty() )
			s += ".I";

		return s;
	}

	void Optimizer::CreateObjectives( size_t count )
	{
		// create at least one objective instance (required for finding number of parameters)
		while ( m_Objectives.size() < count )
			m_Objectives.push_back( CreateObjective( m_ObjectiveProps ) );
	}

	void Optimizer::ManageFileOutput( double fitness, const std::vector< path >& files )
	{
		m_OutputFiles.push_back( std::make_pair( fitness, files ) );
		if ( m_OutputFiles.size() >= 3 )
		{
			// see if we should delete the second last file
			auto testIt = m_OutputFiles.end() - 2;
			double imp1 = testIt->first / ( testIt - 1 )->first;
			double imp2 = ( testIt + 1 )->first / testIt->first;
			if ( !maximize_objective ) { imp1 = 1.0 / imp1; imp2 = 1.0 / imp2; }

			if ( imp1 < min_improvement_factor_for_file_output && imp2 < min_improvement_factor_for_file_output )
			{
				// delete the file(s)
				bool ok = true;
				for ( auto& file : testIt->second )
					ok &= flut::remove( file );

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

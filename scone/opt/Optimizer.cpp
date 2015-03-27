#include "stdafx.h"
#include "Optimizer.h"

#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/thread.hpp>
#include <boost/foreach.hpp>

#include "Factories.h"
#include "../core/Log.h"
#include <sstream>
#include "../core/system.h"

using namespace boost::filesystem;

namespace scone
{
	namespace opt
	{
		Optimizer::Optimizer( const PropNode& props ) :
		max_threads( 1 ),
		thread_priority( 0 ),
		m_ObjectiveProps( props.GetChild( "Objective" ) )
		{
			INIT_FROM_PROP( props, max_threads, 1u );
			INIT_FROM_PROP( props, thread_priority, 0 );
			INIT_FROM_PROP_NAMED( props, m_Name, "name", String() );
			INIT_FROM_PROP( props, maximize_objective, true );
			INIT_FROM_PROP( props, show_optimization_time, false );

			// create at least one objective from props, so that all nodes are properly flagged
			CreateObjectives( 1 );
		}

		Optimizer::~Optimizer()
		{
		}

		// evaluate individuals
		std::vector< double > Optimizer::Evaluate( std::vector< ParamSet >& parsets )
		{
			// run parsets through streams for awesomely reproducibility
			std::vector< ParamSet > newparsets( parsets.size() );
			for ( size_t idx = 0; idx < parsets.size(); ++idx )
			{
				std::stringstream str;
				str << parsets[ idx ];
				str >> newparsets[ idx ];
			}

			// make sure there are enough objectives
			CreateObjectives( parsets.size() );

			if ( max_threads == 1 )
				return EvaluateSingleThreaded( newparsets );
			else return EvaluateMultiThreaded( newparsets );
		}

		// evaluate individuals one-by-one in current thread
		std::vector< double > Optimizer::EvaluateSingleThreaded( std::vector< ParamSet >& parsets )
		{
			std::vector< double > fitnesses( parsets.size(), 999 );
			for ( size_t ind_idx = 0; ind_idx < parsets.size(); ++ind_idx )
			{
				// copy values into par
				fitnesses[ ind_idx ] = m_Objectives[ ind_idx ]->Evaluate( parsets[ ind_idx ] );
				printf(" %3.0f", fitnesses[ ind_idx ] );
			}
			return fitnesses;
		}

		std::vector< double > Optimizer::EvaluateMultiThreaded( std::vector< ParamSet >& parsets )
		{
			// evaluate individuals
			size_t next_idx = 0;
			std::vector< double > fitnesses( parsets.size(), 999 );

			size_t num_active_threads = 0;
			std::vector< std::shared_ptr< boost::thread > > threads( parsets.size(), nullptr );

			while ( num_active_threads > 0 || ( next_idx < parsets.size() ) )
			{
				// add threads
				while ( num_active_threads < max_threads && next_idx < parsets.size() )
				{
					// create new thread at next_idx
					threads[ next_idx ] = std::unique_ptr< boost::thread >( new boost::thread( EvaluateFunc, m_Objectives[ next_idx ].get(), parsets[ next_idx ], &fitnesses[ next_idx ], thread_priority ) );

					num_active_threads++;
					next_idx++;
				}

				// see if threads are finished
				for ( size_t thread_idx = 0; thread_idx < threads.size(); ++thread_idx )
				{
					if ( threads[ thread_idx ] != nullptr && threads[ thread_idx ]->timed_join( boost::posix_time::milliseconds( 100 ) ) )
					{
						// decrease number of active threads
						threads[ thread_idx ].reset();
						num_active_threads--;

						// print some stuff
						printf( "%3.0f ", fitnesses[ thread_idx ] );
					}
				}
			}

			return fitnesses;
		}

		void Optimizer::EvaluateFunc( Objective* obj, ParamSet& par, double* fitness, int priority )
		{
			::SetThreadPriority( ::GetCurrentThread(), priority );
			*fitness = obj->Evaluate( par );
		}

		void Optimizer::InitOutputFolder()
		{
			m_OutputFolder = GetSconeFolder( "output" ) + GetDateTimeAsString() + "." + GetObjective().GetSignature() + "/";
			create_directories( path( m_OutputFolder ) );
			log::Info( ( "Writing to folder " + m_OutputFolder ).c_str() );
		}

		const String& Optimizer::GetOutputFolder()
		{
			if ( m_OutputFolder.empty() )
				InitOutputFolder();
			return m_OutputFolder;
		}

		void Optimizer::CreateObjectives( size_t count )
		{
			// create at least one objective instance (required for finding number of parameters)
			while ( m_Objectives.size() < count )
			{
				m_Objectives.push_back( CreateObjective( m_ObjectiveProps, ParamSet() ) );
				m_Objectives.back()->debug_idx = m_Objectives.size();
			}
		}
	}
}

#include "stdafx.h"
#include "Optimizer.h"

#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/thread.hpp>

namespace scone
{
	namespace opt
	{
		Optimizer::Optimizer() :
		max_threads ( 1 )
		{
		}

		Optimizer::~Optimizer()
		{
		}

		void Optimizer::ProcessProperties( const PropNode& props )
		{
			PROCESS_PROPERTY( props, max_threads );
		}

		// evaluate all individuals
		std::vector< double > Optimizer::EvaluateSingleThreaded( std::vector< ParamSet >& parsets, ObjectiveSP objective )
		{
			std::vector< double > fitnesses( parsets.size(), 999 );
			for ( size_t ind_idx = 0; ind_idx < parsets.size(); ++ind_idx )
			{
				// copy values into par
				fitnesses[ ind_idx ] = objective->Evaluate( parsets[ ind_idx ] );
				printf(" %3.0f", fitnesses[ ind_idx ] );
			}
			return fitnesses;
		}

		std::vector< double > Optimizer::EvaluateMultiThreaded( std::vector< ParamSet >& parsets, ObjectiveSP& objective )
		{
			// evaluate individuals
			size_t next_idx = 0;
			size_t active_threads = 0;
			std::vector< std::shared_ptr< boost::thread > > threads;
			std::vector< double > fitnesses( parsets.size(), 999 );

			while ( active_threads > 0 || ( next_idx < parsets.size() ) )
			{
				// add threads
				while ( active_threads < max_threads && next_idx < parsets.size() )
				{
					std::shared_ptr< boost::thread > t( new boost::thread( EvaluateFunc, objective, parsets[ next_idx ], &fitnesses[ next_idx ] ) );
					threads.push_back( t );
					active_threads++;
					next_idx++;
				}

				// see if threads are finished
				for ( size_t thread_idx = 0; thread_idx < threads.size(); ++thread_idx )
				{
					if ( threads[ thread_idx ] != nullptr && threads[ thread_idx ]->timed_join( boost::posix_time::milliseconds( 100 ) ) )
					{
						// TODO: something with result?

						// decrease number of active threads
						threads[ thread_idx ].reset();
						active_threads--;

						// print some stuff
						printf( "%3.0f ", fitnesses[ thread_idx ] );
					}
				}
			}

			return fitnesses;
		}

		void Optimizer::EvaluateFunc( ObjectiveSP obj, ParamSet& par, double* fitness )
		{
			*fitness = obj->Evaluate( par );
		}
	}
}

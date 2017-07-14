#pragma once

#include "spot/reporter.h"
#include "flut/system/types.hpp"
#include "spot/optimizer.h"
#include "spot/cma_optimizer.h"

namespace scone
{
	class StatusReporter : public spot::reporter
	{
	public:
		StatusReporter() {}
		virtual ~StatusReporter() {}

		virtual void start( const spot::optimizer& opt ) override
		{
			if ( status_output )
			{
				// print out some info
				auto& cma = dynamic_cast< const spot::cma_optimizer& >( opt );
				OutputStatus( "folder", "..." );
				OutputStatus( "dim", opt.info().dim() );
				OutputStatus( "sigma", cma.sigma() );
				OutputStatus( "lambda", cma.lambda() );
				OutputStatus( "mu", cma.mu() );
				OutputStatus( "max_generations", cma.max_steps );
			}
		}

		virtual void finish( const spot::optimizer& opt ) override
		{
			throw std::logic_error( "The method or operation is not implemented." );
		}

		virtual void evaluate( const spot::optimizer& opt, const spot::search_point& point, spot::fitness_t fitness_t ) override
		{
			throw std::logic_error( "The method or operation is not implemented." );
		}

		virtual void evaluate( const spot::optimizer& opt, const spot::search_point_vec& pop, const spot::fitness_vec_t& fitnesses, flut::index_t best_idx, bool new_best ) override
		{
			throw std::logic_error( "The method or operation is not implemented." );
		}

		virtual void new_best( const spot::optimizer& opt, const spot::search_point& point, spot::fitness_t fitness_t ) override
		{
			throw std::logic_error( "The method or operation is not implemented." );
		}

		virtual void next_step( const spot::optimizer& opt, size_t gen ) override
		{
			throw std::logic_error( "The method or operation is not implemented." );
		}

		template< typename T > void OutputStatus( const String& key, const T& value ) { std::cout << std::endl << "*" << key << "=" << value << std::endl; }

		bool show_optimization_time;
		bool console_output;
		bool status_output;
	};
}

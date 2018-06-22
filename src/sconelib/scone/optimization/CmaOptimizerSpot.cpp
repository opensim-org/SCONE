#include "CmaOptimizerSpot.h"

#include <random>

namespace scone
{
	using std::cout;
	using std::endl;

	CmaOptimizerSpot::CmaOptimizerSpot( const PropNode& pn ) :
	CmaOptimizer( pn ),
	spot::cma_optimizer( *m_Objective, lambda_, CmaOptimizer::random_seed )
	{
		size_t dim = GetObjective().dim();
		SCONE_ASSERT( dim > 0 );

		// initialize settings from file
		// TODO: must read parameters before creating cma_optimizer object?
		if ( use_init_file && !init_file.empty() )
		{
			auto result = GetObjective().info().import_mean_std( init_file, use_init_file_std, init_file_std_factor, init_file_std_offset );
			log::info( "Imported ", result.first, ", skipped ", result.second, " parameters from ", init_file );
		}

		if ( global_std_offset != 0.0 || global_std_factor != 0.0 )
			GetObjective().info().set_global_std( global_std_factor, global_std_offset );
		//GetObjective().info().set_minimize( IsMinimizing() );

		lambda_ = lambda();
		mu_ = mu();
		sigma_ = sigma();
		set_max_threads( ( int )max_threads );
		enable_fitness_tracking( window_size );

		add_reporter( std::make_shared< CmaOptimizerReporter >() );
	}

	void CmaOptimizerSpot::Run()
	{
		run( max_generations );
	}

	void CmaOptimizerReporter::start( const optimizer& opt )
	{
		auto& cma = dynamic_cast< const CmaOptimizerSpot& >( opt );

		log::InfoF( "Starting optimization, dim=%d, lambda=%d, mu=%d", cma.dim(), cma.lambda(), cma.mu() );

		if ( cma.GetStatusOutput() )
		{
			// print out some info
			cma.OutputStatus( "folder", cma.AcquireOutputFolder() );
			cma.OutputStatus( "dim", cma.dim() );
			cma.OutputStatus( "sigma", cma.sigma() );
			cma.OutputStatus( "lambda", cma.lambda() );
			cma.OutputStatus( "mu", cma.mu() );
			cma.OutputStatus( "max_generations", cma.max_generations );
			cma.OutputStatus( "window_size", cma.window_size );
		}
	}

	void CmaOptimizerReporter::finish( const optimizer& opt )
	{
		auto& cma = dynamic_cast< const CmaOptimizerSpot& >( opt );

		if ( cma.GetProgressOutput() )
			cout << "Optimization finished" << endl;

		if ( cma.GetStatusOutput() )
			cma.OutputStatus( "finished", 1 );
	}

	void CmaOptimizerReporter::evaluate_population_start( const optimizer& opt, const search_point_vec& pop )
	{
		auto& cma = dynamic_cast< const CmaOptimizerSpot& >( opt );
		if ( cma.GetProgressOutput() )
			printf( "%04d (S=%.3f):", int( opt.current_step() ), cma.sigma() ); // MSVC2013 doesn't support %zu
	}

	void CmaOptimizerReporter::evaluate_population_finish( const optimizer& opt, const search_point_vec& pop, const fitness_vec_t& fitnesses, index_t best_idx, bool new_best )
	{
		auto& cma = dynamic_cast< const CmaOptimizerSpot& >( opt );

		// report results
		if ( cma.GetProgressOutput() )
			printf( " A=%.3f O=%.3f S=%.3f", cma.current_step_average(), cma.fitness_trend().offset(), cma.fitness_trend().slope() );

		if ( cma.GetStatusOutput() )
			cma.OutputStatus( "generation", xo::stringf( "%d %f %f %f %f %f", cma.current_step(), cma.current_step_best(), cma.current_step_median(), cma.current_step_average(), cma.fitness_trend().offset(), cma.fitness_trend().slope() ) );

		if ( new_best )
		{
			if ( cma.GetProgressOutput() )
				printf( " B=%.3f", cma.best_fitness() );
			if ( cma.GetStatusOutput() )
				cma.OutputStatus( "best", cma.best_fitness() );
		}

		if ( new_best || ( cma.current_step() - cma.m_LastFileOutputGen > cma.max_generations_without_file_output ) )
		{
			// copy best solution to par
			ParamInfo parinf( cma.GetObjective().info() );
			parinf.set_mean_std( cma.current_mean(), cma.current_std() );
			ParamInstance par( parinf, pop[ best_idx ].values() );

			cma.m_LastFileOutputGen = cma.current_step();

			// write .par file
			String ind_name = xo::stringf( "%04d_%.3f_%.3f", cma.current_step(), cma.current_step_average(), cma.current_step_best() );
			auto file_base = cma.AcquireOutputFolder() / ind_name;
			std::vector< path > outputFiles;
			std::ofstream( ( file_base + ".par" ).str() ) << par;
			outputFiles.push_back( file_base + ".par" );

			// cleanup superfluous output files
			if ( new_best )
				cma.ManageFileOutput( cma.current_step_best(), outputFiles );
		}

		// show time if needed
		if ( cma.GetProgressOutput() )
			printf( new_best ? "\n" : "\r" ); // only start newline if there's been a new best
	}
}

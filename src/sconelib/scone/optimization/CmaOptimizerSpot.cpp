#include "CmaOptimizerSpot.h"

#include <random>
#include "spot/stop_condition.h"
#include "spot/file_reporter.h"
#include "spot/console_reporter.h"

namespace scone
{
	using std::cout;
	using std::endl;

	CmaOptimizerSpot::CmaOptimizerSpot( const PropNode& pn ) :
	CmaOptimizer( pn ),
	cma_optimizer( *m_Objective, lambda_, CmaOptimizer::random_seed ),
	log_sink_( xo::log::info_level, AcquireOutputFolder() / "optimization.log" )
	{
		size_t dim = GetObjective().dim();
		SCONE_ASSERT( dim > 0 );

		lambda_ = lambda();
		mu_ = mu();
		sigma_ = sigma();
		set_max_threads( ( int )max_threads );
		enable_fitness_tracking( window_size );

		// reporters
		add_reporter< CmaOptimizerReporter >();
		auto& rep = add_reporter< spot::file_reporter >( AcquireOutputFolder() );
		rep.min_improvement_factor_for_file_output = min_improvement_factor_for_file_output;
		if ( GetProgressOutput() )
			add_reporter< spot::console_reporter >();

		// stop conditions
		add_stop_condition< spot::max_steps_condition >( max_generations );
		add_stop_condition< spot::min_progress_condition >( min_progress, min_progress_samples );
	}

	void CmaOptimizerSpot::Run()
	{
		run();
	}

	void CmaOptimizerReporter::on_start( const optimizer& opt )
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

		// setup history.txt
		history_ = std::ofstream( ( cma.AcquireOutputFolder() / "history.txt" ).string() );
		history_ << "Step\tBest\tAverage\tPredicted\tSlope\tOffset\tProgress" << std::endl;
	}

	void CmaOptimizerReporter::on_stop( const optimizer& opt, const spot::stop_condition& s )
	{
		auto& cma = dynamic_cast< const CmaOptimizerSpot& >( opt );

		if ( cma.GetStatusOutput() )
			cma.OutputStatus( "finished", s.what() );
	}

	void CmaOptimizerReporter::on_pre_evaluate_population( const optimizer& opt, const search_point_vec& pop )
	{
		auto& cma = dynamic_cast< const CmaOptimizerSpot& >( opt );
	}

	void CmaOptimizerReporter::on_post_evaluate_population( const optimizer& opt, const search_point_vec& pop, const fitness_vec_t& fitnesses, index_t best_idx, bool new_best )
	{
		auto& cma = dynamic_cast< const CmaOptimizerSpot& >( opt );

		// report results
		if ( cma.GetStatusOutput() )
			cma.OutputStatus( "generation", xo::stringf( "%d %f %f %f %f %f", cma.current_step(), cma.current_step_best(), cma.current_step_median(), cma.current_step_average(), cma.fitness_trend().offset(), cma.fitness_trend().slope() ) );

		if ( new_best )
		{
			if ( cma.GetStatusOutput() )
				cma.OutputStatus( "best", cma.best_fitness() );
		}

#if 0
		// output file
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

		// update history
		auto cur_trend = opt.fitness_trend();
		auto max_steps = opt.find_stop_condition< spot::max_steps_condition >().max_steps_;
		history_ << opt.current_step()
			<< "\t" << opt.current_step_best()
			<< "\t" << opt.current_step_average()
			<< "\t" << opt.predicted_fitness( max_steps )
			<< "\t" << cur_trend.slope()
			<< "\t" << cur_trend.offset()
			<< "\t" << opt.progress() << "\n";
		if ( opt.current_step() % 10 == 9 ) // flush every 10 entries
			history_.flush();
#endif
	}
}

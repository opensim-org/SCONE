#include "OptimizerTaskThreaded.h"

#include "xo/serialization/serialize.h"
#include "scone/core/Factories.h"
#include "scone/optimization/opt_tools.h"
#include "spot/optimizer.h"

namespace scone
{
	OptimizerTaskThreaded::OptimizerTaskThreaded( const QString& scenario, const QStringList& options ) :
		OptimizerTask( scenario, options ),
		has_optimizer_( false ),
		active_( true )
	{
		xo::path scenario_path( scenario_file_.toStdString() );
		scenario_pn_ = xo::load_file_with_include( scenario_path, "INCLUDE" );

		// apply command line settings (parameter 2 and further)
		for ( const auto& kvstring : options )
		{
			auto kvp = xo::make_key_value_str( kvstring.toStdString() );
			scenario_pn_.set_query( kvp.first, kvp.second, '.' );
		}

		thread_ = std::thread( &OptimizerTaskThreaded::thread_func, this );
	}

	OptimizerTaskThreaded::~OptimizerTaskThreaded()
	{
		if ( active_ )
		{
			log::warning( "Destroying task while optimizer is active" );
			interrupt();
		}

		if ( thread_.joinable() )
			thread_.join();
	}

	void OptimizerTaskThreaded::thread_func()
	{
		// do optimization
		xo::path scenario_dir = xo::path( scenario_file_.toStdString() ).parent_path();
		optimizer_ = CreateOptimizer( scenario_pn_, scenario_dir );
		has_optimizer_ = true; // set the flag because unique_ptr can't be atomic
		optimizer_->SetOutputMode( Optimizer::status_queue_output );
		optimizer_->Run();
		active_ = false;
	}

	bool OptimizerTaskThreaded::interrupt()
	{
		if ( has_optimizer_ )
		{
			dynamic_cast<spot::optimizer&>( *optimizer_ ).interrupt();
			return true;
		}
		else return false;
	}

	void OptimizerTaskThreaded::finish()
	{
		thread_.join();
	}

	std::deque<PropNode> OptimizerTaskThreaded::getMessages()
	{
		std::deque<PropNode> results;
		if ( has_optimizer_ )
			return results = optimizer_->GetStatusMessages();
		return results;
	}
}

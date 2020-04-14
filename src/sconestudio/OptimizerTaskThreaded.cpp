#include "OptimizerTaskThreaded.h"

#include "xo/serialization/serialize.h"
#include "scone/core/Factories.h"
#include "scone/optimization/opt_tools.h"
#include "spot/optimizer.h"

namespace scone
{
	OptimizerTaskThreaded::OptimizerTaskThreaded( const QString& scenario, const QStringList& options ) :
		OptimizerTask( scenario, options ),
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

		// do optimization
		optimizer_ = CreateOptimizer( scenario_pn_, scenario_path.parent_path() );
		optimizer_->SetOutputMode( Optimizer::status_queue_output );
		thread_ = std::thread( [&]() { optimizer_->Run(); active_ = false; } );
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

	void OptimizerTaskThreaded::interrupt()
	{
		dynamic_cast<spot::optimizer&>( *optimizer_ ).interrupt();
	}

	void OptimizerTaskThreaded::waitUntilDone()
	{
		thread_.join();
	}

	bool OptimizerTaskThreaded::isActive()
	{
		return active_;
	}

	xo::optional<PropNode> OptimizerTaskThreaded::tryGetMessage( xo::error_code* ec )
	{
		return optimizer_->TryPopStatus();
	}
}

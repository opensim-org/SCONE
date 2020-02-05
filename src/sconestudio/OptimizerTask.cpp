#include "OptimizerTask.h"

namespace scone
{
	OptimizerTask::OptimizerTask( const QString& scenario, const QStringList& options ) :
		scenario_file_( scenario ),
		options_( options )
	{}

	OptimizerTask::~OptimizerTask()
	{}
}

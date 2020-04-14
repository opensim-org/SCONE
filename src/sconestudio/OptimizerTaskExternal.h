#pragma once

#include <QProcess>
#include "OptimizerTask.h"
#include "xo/filesystem/path.h"
#include "scone/core/types.h"

namespace scone
{
	class OptimizerTaskExternal : public OptimizerTask
	{
	public:
		OptimizerTaskExternal( const QString& scenario, const QStringList& options = QStringList() );
		virtual ~OptimizerTaskExternal();
		
		void interrupt() override;
		void waitUntilDone() override;
		bool isActive() override;
		xo::optional<PropNode> tryGetMessage( xo::error_code* ec ) override;

	protected:
		QProcess* process_;
	};
}

#pragma once

#include <QStringList>
#include <QString>

#include "scone/core/Exception.h"
#include "scone/core/PropNode.h"
#include "xo/system/error_code.h"
#include "xo/filesystem/path.h"
#include "xo/utility/optional.h"
#include "scone/core/types.h"

namespace scone
{
	class OptimizerTask
	{
	public:
		OptimizerTask( const QString& scenario, const QStringList& args );
		virtual ~OptimizerTask();

		virtual bool interrupt() = 0;
		virtual void finish() = 0;
		virtual xo::optional<PropNode> tryGetMessage( xo::error_code* ec ) = 0;

		const QString scenario_file_;
		const QStringList options_;
	};

	u_ptr<OptimizerTask> createOptimizerTask( const QString& scenario, const QStringList& args = QStringList() );
}

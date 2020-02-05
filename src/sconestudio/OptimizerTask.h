#pragma once

#include <QStringList>
#include <QString>

#include "scone/core/Exception.h"
#include "scone/core/PropNode.h"
#include "xo/system/error_code.h"
#include "xo/filesystem/path.h"
#include "scone/core/types.h"

namespace scone
{
	class OptimizerTask
	{
	public:
		OptimizerTask( const QString& scenario, const QStringList& args );
		virtual ~OptimizerTask();

		virtual void close() = 0;
		virtual bool isActive() = 0;
		virtual bool hasMessage() = 0;
		virtual PropNode message( xo::error_code* ec ) = 0;

		const QString scenario_file_;
		const QStringList options_;
	};
}

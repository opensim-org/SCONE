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
		
		bool interrupt() override;
		void finish() override;
		std::deque<PropNode> getMessages() override;

	protected:
		QProcess* process_;
		bool send_process_closed_mesage_;
	};
}

#pragma once

#include "OptimizerTask.h"
#include <thread>

namespace scone
{
	class OptimizerTaskThreaded : public OptimizerTask
	{
	public:
		OptimizerTaskThreaded( const QString& scenario, const QStringList& options = QStringList() );
		virtual ~OptimizerTaskThreaded();

		virtual void interrupt() override;
		virtual void waitUntilDone() override;
		virtual bool isActive() override;

		xo::optional<PropNode> tryGetMessage( xo::error_code* ec ) override;

	protected:
		PropNode scenario_pn_;
		OptimizerUP optimizer_;
		std::thread thread_;
		std::atomic_bool active_;
	};
}

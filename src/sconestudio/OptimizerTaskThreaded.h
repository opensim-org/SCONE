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

		void close() override;
		bool isActive() override;

		xo::optional<PropNode> tryGetMessage( xo::error_code* ec ) override;

	protected:
		PropNode scenario_pn_;
		OptimizerUP optimizer_;
		std::thread thread_;
	};
}

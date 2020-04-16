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

		virtual bool interrupt() override;
		virtual void finish() override;

		std::deque<PropNode> getMessages() override;

	protected:
		void thread_func();

		PropNode scenario_pn_;
		OptimizerUP optimizer_;
		std::atomic_bool has_optimizer_; // required because unique_ptr can't be atomic
		std::thread thread_;
		std::atomic_bool active_;
	};
}

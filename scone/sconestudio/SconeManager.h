#pragma once

#include "scone/core/core.h"
#include "scone/sim/sim.h"
#include "scone/core/PropNode.h"
#include "scone/core/Storage.h"
#include "scone/opt/opt.h"

namespace scone
{
	class SconeManager
	{
	public:
		SconeManager();
		virtual ~SconeManager();

		void InitParFile( const String& file );
		void Evaluate( TimeInSeconds endtime = 0.0 );
		const Storage< Real, TimeInSeconds >& GetData() { return data; }

	private:
		Storage< Real, TimeInSeconds > data;
		String filename;
		opt::ObjectiveUP objective;
		PropNode statistics;
	};
}

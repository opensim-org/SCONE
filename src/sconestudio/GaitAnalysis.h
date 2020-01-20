#pragma once

#include <QWidget>
#include "scone/core/Storage.h"

namespace scone
{
	class GaitAnalysis
	{
	public:
		GaitAnalysis() {}
		virtual ~GaitAnalysis() {}

	private:
		Storage<> sto_;
	};
}

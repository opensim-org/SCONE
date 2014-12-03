#pragma once

#include <memory>

#ifdef OPT_EXPORTS
#define OPT_API __declspec(dllexport)
#else
#define OPT_API __declspec(dllimport)
#endif

namespace scone
{
	namespace opt
	{
		// forward declarations
		class Param;
		class ParamSet;
		class Optimizer;
		class Objective;

		// pointer types
		typedef std::shared_ptr< Optimizer > OptimizerSP;
		typedef std::shared_ptr< Objective > ObjectiveSP;
	}
}

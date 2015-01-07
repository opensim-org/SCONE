#pragma once

#include <memory>
#include "../core/core.h"
#include "../core/PropNode.h"

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
		SCONE_DECLARE_CLASS_AND_PTR( Optimizer );
		SCONE_DECLARE_CLASS_AND_PTR( Objective );

		class Param;
		class ParamSet;

		// register factory types
		void OPT_API RegisterFactoryTypes();
		OptimizerUP OPT_API CreateOptimizerFromXml( const String& xml_file, const String& key = "Optimizer" );
	}
}

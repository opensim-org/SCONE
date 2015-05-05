#pragma once

#include <memory>
#include "../core/core.h"
#include "../core/PropNode.h"
#include "../core/tools.h"

#ifdef OPT_EXPORTS
#define OPT_API __declspec(dllexport)
#else
#define OPT_API __declspec(dllimport)
#endif

#define INIT_FROM_PROP_PAR( PROP_, PAR_, VAR_, DEFAULT_ ) \
	VAR_ = PROP_.HasKey( GetCleanVarName( #VAR_ ) ) ? PAR_.Get( GetCleanVarName( #VAR_ ), PROP_.GetChild( GetCleanVarName( #VAR_ ) ) ) : DEFAULT_

#define INIT_FROM_PROP_PAR_REQUIRED( PROP_, PAR_, VAR_ ) \
	VAR_ = PAR_.Get( GetCleanVarName( #VAR_ ), PROP_.GetChild( GetCleanVarName( #VAR_ ) ) )

namespace scone
{
	namespace opt
	{
		// forward declarations
		SCONE_DECLARE_CLASS_AND_PTR( Optimizer );
		SCONE_DECLARE_CLASS_AND_PTR( Objective );
		SCONE_DECLARE_CLASS_AND_PTR( Param );
		SCONE_DECLARE_CLASS_AND_PTR( ParamSet );

		// register factory types
		void OPT_API RegisterFactoryTypes();

		// TODO: move this to sconeopt?
		void OPT_API PerformOptimization( int argc, char* argv[] );
	}
}

/*
** Profiler.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "platform.h"
#include "PropNode.h"
#include "scone/core/string_tools.h"
#include "xo/time/timer.h"

#include <map>
#include "xo/diagnose/profiler.h"

#if defined SCONE_ENABLE_PROFILING
#	define SCONE_PROFILE_FUNCTION ScopedProfile unique_scoped_function_profile( Profiler::GetGlobalInstance(), __FUNCTION__ )
#	define SCONE_PROFILE_SCOPE( scope_name_arg ) ScopedProfile unique_scoped_function_profile( Profiler::GetGlobalInstance(), scope_name_arg )
#	define SCONE_PROFILE_RESET Profiler::GetGlobalInstance().Reset()
#	define SCONE_PROFILE_REPORT log::info( Profiler::GetGlobalInstance().GetReport() )
#elif defined SCONE_ENABLE_XO_PROFILING
#	define SCONE_PROFILE_FUNCTION xo::scoped_section_profiler unique_scoped_profile( __FUNCTION__ )
#	define SCONE_PROFILE_SCOPE( scope_name_arg ) xo::scoped_section_profiler unique_scoped_profile( scope_name_arg )
#	define SCONE_PROFILE_RESET xo::profiler::instance().reset()
#	define SCONE_PROFILE_REPORT log::info( xo::profiler::instance().report() )
#else 
#	define SCONE_PROFILE_FUNCTION
#	define SCONE_PROFILE_SCOPE( scope_name_arg )
#	define SCONE_PROFILE_RESET
#	define SCONE_PROFILE_REPORT PropNode()
#endif

namespace scone
{
	typedef long long HighResolutionTime;

	class SCONE_API ScopedProfile
	{
	public:
		ScopedProfile( class Profiler& prof, const String& name );
		~ScopedProfile();

	private:
		class Profiler& m_Profiler;
		HighResolutionTime m_StartTime;
	};

	class SCONE_API Profiler
	{
	public:
		friend class ScopedProfile;

		Profiler();
		Profiler( const Profiler& other ) = delete;
		Profiler& operator=( const Profiler& other ) = delete;
		virtual ~Profiler();

		PropNode GetReport();
		void Activate();
		void Suspend();
		bool IsActive();
		void Reset();

		static Profiler& GetGlobalInstance();

	private:
		HighResolutionTime StartMeasure( const String& scope );
		void StopMeasure( HighResolutionTime start_time );

		// TODO: move to pImpl
		struct Item {
			Item( Item* parent );
			void AddSample( HighResolutionTime duration );
			Item& GetOrAddChild( const String& scope );
			HighResolutionTime GetReport( PropNode& pn );
			size_t num_samples;
			HighResolutionTime inclusive_time;
			HighResolutionTime peak_time;
			Item* parent;
			std::map< String, std::unique_ptr< Item > > children;
		};

		Item m_Root;
		Item* m_Current;
		bool m_bActive;
		xo::timer m_Timer;
	};
}

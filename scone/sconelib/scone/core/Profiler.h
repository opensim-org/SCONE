#pragma once

#include "core.h"
#include "PropNode.h"
#include "scone/core/string_tools.h"
#include <flut/timer.hpp>

#include <map>
#include "flut/system/profiler.hpp"

#if defined SCONE_ENABLE_PROFILING
#	define SCONE_PROFILE_FUNCTION ScopedProfile unique_scoped_function_profile( Profiler::GetGlobalInstance(), __FUNCTION__ )
#	define SCONE_PROFILE_SCOPE( scope_name_arg ) ScopedProfile unique_scoped_function_profile( Profiler::GetGlobalInstance(), scope_name_arg )
#	define SCONE_PROFILE_RESET Profiler::GetGlobalInstance().Reset()
#	define SCONE_PROFILE_REPORT Profiler::GetGlobalInstance().GetReport()
#elif defined SCONE_ENABLE_FLUT_PROFILING
#	define SCONE_PROFILE_FUNCTION flut::profile_section unique_scoped_profile( __FUNCTION__ )
#	define SCONE_PROFILE_SCOPE( scope_name_arg ) flut::profile_section unique_scoped_profile( scope_name_arg )
#	define SCONE_PROFILE_RESET flut::profiler::instance().reset()
#	define SCONE_PROFILE_REPORT flut::profiler::instance().report()
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
		flut::timer m_Timer;

	private:
		Profiler( const Profiler& other );
		Profiler& operator=( const Profiler& other );
	};
}

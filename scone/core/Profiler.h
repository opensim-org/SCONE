#pragma once

#include "core.h"
#include "PropNode.h"
#include <map>
#include "Timer.h"

//#define SCONE_ENABLE_PROFILING

#ifdef SCONE_ENABLE_PROFILING
	#define SCONE_PROFILE_SCOPE ScopedProfile unique_scoped_function_profile( Profiler::GetGlobalInstance(), __FUNCTION__ )
	#define SCONE_PROFILE_SCOPE_NAMED( scope_name_arg ) ScopedProfile unique_scoped_function_profile( Profiler::GetGlobalInstance(), scope_name_arg )
#else 
	#define SCONE_PROFILE_SCOPE
	#define SCONE_PROFILE_SCOPE_NAMED( scope_name_arg )
#endif

namespace scone
{
	class Profiler;

	class CORE_API ScopedProfile
	{
	public:
		ScopedProfile( Profiler& prof, const String& name );
		~ScopedProfile();

	private:
		class Profiler& m_Profiler;
		long long m_Time;
	};

	class CORE_API Profiler
	{
	public:
		friend class ScopedProfile;
		//typedef LONGLONG Ticks;

		Profiler();
		virtual ~Profiler();

		PropNode GetReport();
		void Activate();
		void Suspend();
		bool IsActive();
		static Profiler& GetGlobalInstance();

	private:
		long long StartMeasure( const String& scope );
		void StopMeasure( long long start_time );

		// TODO: move to pImpl
		struct Item {
			Item( Item* parent );
			void AddSample( long long duration );
			Item& GetOrAddChild( const String& scope );
			long long GetReport( PropNode& pn );
			size_t num_samples;
			long long inclusive_time;
			long long peak_time;
			Item* parent;
			std::map< String, std::unique_ptr< Item > > children;
		};

		Item m_Root;
		Item* m_Current;
		bool m_bActive;
	};

}

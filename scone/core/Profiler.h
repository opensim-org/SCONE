#pragma once

#include "core.h"
#include "PropNode.h"
#include <map>
#include "Timer.h"

namespace scone
{
	class CORE_API ScopedProfile
	{
	public:
		ScopedProfile( class Profiler& prof, const String& name );
		~ScopedProfile();

	private:
		class Profiler& m_Profiler;
		TimeInSeconds m_Time;
	};

	class CORE_API Profiler
	{
	public:
		friend class ScopedProfile;

		Profiler();
		virtual ~Profiler();

		PropNode GetReport();

	private:

		TimeInSeconds StartMeasure( const String& scope );
		void StopMeasure( TimeInSeconds start_time );

		struct Item {
			Item( Item* parent );
			void AddSample( TimeInSeconds time );
			Item& GetOrAddChild( const String& scope );
			TimeInSeconds GetReport( PropNode& pn );
			size_t num_samples;
			TimeInSeconds inclusive_time;
			TimeInSeconds peak_time;
			Item* parent;
			std::map< String, std::unique_ptr< Item > > children;
		};

		Item m_Root;

		Timer m_Timer;
		Item* m_Current;
	};
}

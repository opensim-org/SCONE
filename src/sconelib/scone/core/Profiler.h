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

		// #todo: move to pImpl
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

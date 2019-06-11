/*
** TimeStateController.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "StateController.h"

namespace scone
{
	class TimeStateController : public StateController
	{
	public:
		TimeStateController( const PropNode& props, Params& par, Model& model, const Location& loc );

		virtual size_t GetStateCount() const override { return m_States.size(); }
		virtual const String& GetStateName( StateIndex i ) const override { return m_States[ i ].name; }

	protected:
		virtual String GetClassSignature() const override;
		virtual StateIndex GetCurrentState( Model& model, double timestamp ) override;

		struct TimeState
		{
			TimeState( const PropNode& pn, Params& par );
			bool operator<( const TimeState& other ) const { return start_time < other.start_time; }
			String name;
			TimeInSeconds start_time;
		};

		std::vector< TimeState > m_States;
	};
}

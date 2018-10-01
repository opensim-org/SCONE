/*
** FeedForwardController.h
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "scone/core/types.h"
#include "scone/controllers/Controller.h"
#include "scone/core/PropNode.h"
#include "scone/optimization/Params.h"
#include "scone/core/Function.h"
#include "scone/model/Leg.h"

namespace OpenSim
{
	class PiecewiseLinearFunction;
}

namespace scone
{
	/// Controller that produces a function-based feed-forward signal for each actuator.
	class FeedForwardController : public Controller
	{
	public:
		FeedForwardController( const PropNode& props, Params& par, Model& model, const Location& target_area );
		virtual ~FeedForwardController() { };

		/// Parameterizable Function to be used as feed-forward signal; see Function.
		const PropNode& Function;

		/// Bool indicating if function should be the same for left and right; default = true.
		bool symmetric;

		virtual bool ComputeControls( Model& model, double timestamp ) override;
		virtual String GetClassSignature() const override;

	private:
		// muscle info
		struct ActInfo
		{
			ActInfo() : side( NoSide ), function_idx( NoIndex ), muscle_idx( NoIndex ) {};
			String name;
			Side side;
			String full_name;
			size_t function_idx;
			size_t muscle_idx;
			std::vector< double > mode_weights;
		};

		std::vector< FunctionUP > m_Functions;
		std::vector< ActInfo > m_ActInfos;

	private: // non-copyable and non-assignable
		FeedForwardController( const FeedForwardController& );
		FeedForwardController& operator=( const FeedForwardController& );
	};
}

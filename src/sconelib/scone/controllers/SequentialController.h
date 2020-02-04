/*
** SequentialController.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "CompositeController.h"

namespace scone
{
	/// Controller that activates individual Controllers in sequential order.
	/** The transition between the individual Controllers is governed by ''transition_intervals''
	parameter, which is an array of intervals between Controllers. The number of components in
	''transition_intervals'' should be equal to ''number of child controllers - 1''. <b>Important:</b>
	you should add a <b>name</b> your child controllers to ensure the optimization parameters have
	unique names.
	\verbatim
	# Example of a 2-step feed-forward jumping controller using a SequentialController
	SequentialController {
		# We have two controllers, so one interval
		transition_intervals = [ ~0.15<0.1,0.5> ] 

		# First controller prepares for jumping
		FeedForwardController {
			name = Prepare
			symmetric = 1
			Polynomial { degree = 0 coefficient0 = 0.2~0.01<0,1> }
		}

		# Second controller does the actual jump
		FeedForwardController {
			name = Jump
			symmetric = 1
			Polynomial { degree = 0 coefficient0 = 0.5~0.1<0,1> }
		}
	}
	\endverbatim
	*/
	class SequentialController : public CompositeController
	{
	public:
		SequentialController( const PropNode& props, Params& par, Model& model, const Location& loc );
		virtual ~SequentialController() {}

		/// intervals to use between each set of consecutive controllers
		std::vector< TimeInSeconds > transition_intervals;

		virtual bool ComputeControls( Model& model, double timestamp ) override;

	protected:
		virtual bool PerformAnalysis( const Model& model, double timestamp ) override;
		index_t GetActiveIdx( double timestamp );
		virtual String GetClassSignature() const override;
		std::vector< TimeInSeconds > transition_times;
	};
}

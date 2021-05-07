/*
** BalanceMeasure.h
**
** Copyright (C) 2013-2021 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "Measure.h"

namespace scone
{
	/// Measure that checks for balance, i.e. if a specific vertical COP position is maintained.
	class BalanceMeasure : public Measure
	{
	public:
		BalanceMeasure( const PropNode& props, Params& par, const Model& model, const Location& loc );

		/// Relative COM height (factor of initial COM height) at which to stop the simulation; default = 0.5.
		double termination_height;

		virtual bool UpdateMeasure( const Model& model, double timestamp ) override;
		virtual double ComputeResult( const Model& model ) override;

	protected:
		virtual String GetClassSignature() const override;

	private:
		double m_InitialHeight;
	};
}

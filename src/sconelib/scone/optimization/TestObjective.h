/*
** TestObjective.h
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "Objective.h"

namespace scone
{
	/// Objective used for testing, evaluates a multi-dimensional Schwefel function.
	class SCONE_API TestObjective : public Objective
	{
	public:
		TestObjective( const PropNode& pn );
		virtual ~TestObjective() {}

		/// Dimension of the objective function
		size_t dim_;

		virtual fitness_t evaluate( const SearchPoint& point ) const override;

	protected:
		virtual String GetClassSignature() const override;

	};
}

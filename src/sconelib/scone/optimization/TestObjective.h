/*
** TestObjective.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
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
		TestObjective( const PropNode& pn, const path& find_file_folder );
		virtual ~TestObjective() {}

		/// Dimension of the objective function
		size_t dim_;

		virtual fitness_t evaluate( const SearchPoint& point ) const override;

	protected:
		virtual String GetClassSignature() const override;

	};
}

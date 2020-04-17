/*
** Objective.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "Params.h"
#include "scone/core/HasSignature.h"
#include "spot/objective.h"
#include "scone/core/system_tools.h"
#include "scone/core/HasExternalResources.h"

namespace scone
{
	using spot::fitness_t;
	using spot::result;

	/// Base class for Objectives.
	class SCONE_API Objective : public HasSignature, public HasExternalResources, public spot::objective
	{
	public:
		Objective( const PropNode& props, const path& external_resource_dir );
		virtual ~Objective();

		// write results and return all files written
		virtual std::vector< path > WriteResults( const path& file_base ) { return std::vector< path >(); }
		const path& GetExternalResourceDir() const { return external_resource_dir_; }
		void SetExternalResourceDir( const path& dir ) { external_resource_dir_ = dir; }

	protected:
		// this is where external resources of the objective reside
		path external_resource_dir_;
	};
}

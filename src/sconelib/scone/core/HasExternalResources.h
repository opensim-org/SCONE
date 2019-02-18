/*
** HasExternalResources.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "platform.h"
#include <vector>
#include "xo/filesystem/path.h"

namespace scone
{
	class SCONE_API HasExternalResources
	{
	public:
		HasExternalResources() {}
		inline virtual ~HasExternalResources() {}
		const std::vector< xo::path >& GetExternalResources() const { return external_resources_; }
		void AddExternalResource( const xo::path& p ) { external_resources_.emplace_back( p ); }
		void AddExternalResources( const std::vector< xo::path >& p ) { external_resources_.insert( external_resources_.end(), p.begin(), p.end() ); }
		
	protected:
		std::vector< xo::path > external_resources_;
	};
}

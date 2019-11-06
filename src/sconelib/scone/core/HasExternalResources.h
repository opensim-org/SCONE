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
		HasExternalResources();
		virtual ~HasExternalResources();
		const std::vector< xo::path >& GetExternalResources() const;
		void AddExternalResource( const xo::path& p );
		void AddExternalResources( const HasExternalResources& other );
		
	protected:
		std::vector< xo::path > external_resources_;
	};
}

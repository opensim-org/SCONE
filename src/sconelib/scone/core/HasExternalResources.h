#pragma once

#include "core.h"
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

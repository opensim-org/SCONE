#include "HasExternalResources.h"

#include "xo/container/container_tools.h"

namespace scone
{
	HasExternalResources::HasExternalResources() {}

	HasExternalResources::~HasExternalResources() {}

	const std::vector<xo::path>& HasExternalResources::GetExternalResources() const
	{
		return external_resources_;
	}

	void HasExternalResources::AddExternalResource( const xo::path& p )
	{
		external_resources_.emplace_back( p );
	}

	void HasExternalResources::AddExternalResources( const HasExternalResources& other )
	{
		xo::append( external_resources_, other.external_resources_ );
	}
}

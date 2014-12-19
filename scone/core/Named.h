#pragma once

#include "core.h"
#include "Exception.h"

namespace scone
{
	class CORE_API Named
	{
	public:
		Named();
		virtual ~Named();
		virtual const String& GetName() { SCONE_THROW_NOT_IMPLEMENTED; }
		virtual void SetName( const String& ) { SCONE_THROW_NOT_IMPLEMENTED; }
	};
}


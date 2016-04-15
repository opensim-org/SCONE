#pragma once

#include "Storage.h"

namespace scone
{
	class HasData
	{
	public:
		virtual ~HasData() {}
		virtual void StoreData( Storage< Real >::Frame& frame ) = 0;
	};
}

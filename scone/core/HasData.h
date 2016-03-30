#pragma once

#include "Storage.h"

namespace scone
{
	class HasData
	{
	public:
		virtual ~HasData() = 0 {}
		virtual void StoreData( Storage< Real >::Frame& frame ) = 0;
	};
}

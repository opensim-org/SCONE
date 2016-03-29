#pragma once

#include "Storage.h"

namespace scone
{
	class CORE_API HasData
	{
	public:
		virtual ~HasData();
		virtual void StoreData( Storage< Real >::Frame& frame ) = 0;
	};
}

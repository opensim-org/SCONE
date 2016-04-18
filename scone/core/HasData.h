#pragma once

#include "Storage.h"

namespace scone
{
	class CORE_API HasData
	{
	public:
		virtual void StoreData( Storage< Real >::Frame& frame ) = 0;
		inline virtual ~HasData() {}
	};
}

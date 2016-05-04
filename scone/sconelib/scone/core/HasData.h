#pragma once

#include "Storage.h"

namespace scone
{
	class SCONE_API HasData
	{
	public:
		virtual void StoreData( Storage< Real >::Frame& frame ) = 0;
		inline virtual ~HasData() {}
	};
}

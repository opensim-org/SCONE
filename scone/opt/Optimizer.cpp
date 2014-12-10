#include "stdafx.h"
#include "Optimizer.h"

namespace scone
{
	namespace opt
	{
		Optimizer::Optimizer() :
		max_threads ( 1 )
		{
		}

		Optimizer::~Optimizer()
		{
		}

		void Optimizer::ProcessProperties( const PropNode& props )
		{
			PROCESS_PROPERTY( props, max_threads );
		}
	}
}

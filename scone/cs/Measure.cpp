#include "stdafx.h"
#include "Measure.h"

namespace scone
{
	namespace cs
	{
		void Measure::ProcessPropNode( PropNode& props )
		{
			PROCESS_PROP( props, m_Weight );
		}
	}
}

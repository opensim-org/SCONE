#include "stdafx.h"
#include "ParamSet.h"
#include "..\core\Exception.h"
#include <algorithm>

namespace scone
{
	namespace opt
	{
		void ParamSet::ProcessParameter( double& par, const String& name, double init_mean, double init_var, double min, double max )
		{
			if ( m_Mode == CONSTRUCTION_MODE )
			{
				if ( m_ParamInfos.find( name ) != m_ParamInfos.end() )
					SCONE_THROW( "Duplicate parameter: " + name );

				// add parameter description
				m_ParamInfos[ name ] = ParamInfo( m_ParamInfos.size(), init_mean, init_var, min, max );
			}
			else if ( m_Mode == UPDATE_MODE )
			{
				auto iter = m_ParamInfos.find( name );
				if ( iter == m_ParamInfos.end() )
					SCONE_THROW( "Could not find parameter: " + name );

				// update parameter value
				par = m_ParamValues[ iter->second.index ];
			}
		}
	}
}

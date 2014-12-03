#pragma once

#include "..\core\core.h"
#include "Param.h"
#include <map>

namespace scone
{
	namespace opt
	{
		class ParamSet
		{
		public:
			ParamSet() { };
			virtual ~ParamSet() { };

			void SetParam( const String& name, const Param& param );
			void GetParam( const String& name, Param& param );

			size_t GetSize() { return m_Params.size(); }

			std::vector< double > GetParamValues();
			void SetParamValues( const std::vector< double >& values );
			
		private:
			std::map< String, Param > m_Params;
		};
	}
}

#pragma once

#include "..\core\core.h"
#include "Param.h"
#include "opt.h"
#include <vector>

namespace scone
{
	namespace opt
	{
		class OPT_API ParamSet
		{
		public:
			typedef enum Mode { CONSTRUCTION_MODE, UPDATE_MODE };

			ParamSet( Mode m = UPDATE_MODE ) : m_Mode( m ) { };
			virtual ~ParamSet() { };

			void ProcessParam( const String& name, double& value, double init_mean, double init_var, double min, double max );

			void SetParam( const String& name, const Param& param );
			void GetParam( const String& name, Param& param );

			size_t GetSize() { return m_Params.size(); }

			std::vector< double > GetParamValues();
			void SetParamValues( const std::vector< double >& values );
			
		private:
			Mode m_Mode;
			std::vector< std::pair< String, Param > > m_Params;
		};

		class OPT_API Parameterizable
		{
		public:
			Parameterizable() { };
			virtual ~Parameterizable() { };

			virtual void ProcessParamSet( ParamSet& par ) = 0;
		};
	}
}

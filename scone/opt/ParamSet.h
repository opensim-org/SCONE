#pragma once

#include "..\core\core.h"
#include "Param.h"
#include "opt.h"
#include <vector>
#include <map>

namespace scone
{
	namespace opt
	{
		class OPT_API ParamSet
		{
		public:
			struct OPT_API ParamInfo
			{
			public:
				ParamInfo( size_t i_index = NO_INDEX, double i_mean = 0.0, double i_var = 0.0, double i_min = REAL_MIN, double i_max = REAL_MAX ) :
					index( i_index), init_mean( i_mean ), init_var( i_var ), min( i_min ), max( i_max ) { };

				size_t index;
				double init_mean;
				double init_var;
				double min;
				double max;
			};

			typedef enum Mode { CONSTRUCTION_MODE, UPDATE_MODE };
			ParamSet( Mode m = UPDATE_MODE ) : m_Mode( m ) { };
			virtual ~ParamSet() { };

			void ProcessParameter( double& par, const String& name, double init_mean, double init_var, double min, double max );

			std::vector< double > GetParamValues();
			void SetParamValues( const std::vector< double >& values );

		private:
			Mode m_Mode;
			std::map< String, ParamInfo > m_ParamInfos;
			std::vector< double > m_ParamValues;
		};

		class OPT_API Parameterizable
		{
		public:
			Parameterizable() { };
			virtual ~Parameterizable() { };

			virtual void ProcessParameters( ParamSet& par ) = 0;
		};
	}
}

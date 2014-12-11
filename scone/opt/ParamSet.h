#pragma once

#include "..\core\core.h"
#include "ParamInfo.h"
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
			typedef enum Mode { CONSTRUCTION_MODE, UPDATE_MODE };
			ParamSet( Mode m = UPDATE_MODE ) : m_Mode( m ) { };
			virtual ~ParamSet() { };

			void ProcessParameter( double& par, const ParamInfo& info );
			void ProcessParameter( double& par, const String& name, double init_mean, double init_var, double min, double max );

			bool CheckValues();
			void RestrainValues();
			void InitRandom();

			size_t GetFreeParamCount();
			std::vector< double > GetFreeParamValues();
			void SetFreeParamValues( std::vector< double >& values );

			void SetMode( Mode m ) { m_Mode = m; }

		private:
			Mode m_Mode;
			std::vector< std::pair< ParamInfo, double > > m_Params;
			std::vector< std::pair< ParamInfo, double > >::iterator FindParamByName( const String& name );
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

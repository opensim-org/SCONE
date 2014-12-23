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

			double Get( const ParamInfo& info );
			double GetMeanStd( const String& name, double init_mean, double init_std, double min, double max );
			double GetMinMax( const String& name, double init_min, double init_max, double min, double max );
			double operator()( const String& name, double init_mean, double init_std, double min, double max );
			double operator()( const String& name, double min, double max );

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

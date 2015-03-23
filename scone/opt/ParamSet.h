#pragma once

#include "../core/core.h"
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
			typedef enum Mode { ConstructionMode, UpdateMode };
			ParamSet( Mode m = ConstructionMode ) : m_Mode( m ) { };
			ParamSet( const String& filename );
			virtual ~ParamSet() { };

			double Get( const ParamInfo& info );
			double Get( const String& name, const PropNode& props );
			double GetMeanStd( const String& name, double init_mean, double init_std, double min, double max );
			double GetMinMax( const String& name, double init_min, double init_max, double min, double max );

			bool CheckValues();
			void RestrainValues();
			void InitRandom();

			void UpdateMeanStd( const std::vector< ParamSet >& parsets );

			size_t GetFreeParamCount();
			std::vector< double > GetFreeParamValues();
			void SetFreeParamValues( std::vector< double >& values );

			void SetMode( Mode m ) { m_Mode = m; }
			bool IsInConstructionMode() { return m_Mode == ConstructionMode; }

			void Write( const String& filename );
			void Read( const String& filename );

			void PushNamePrefix( const String& prefix );
			void PopNamePrefix();
			String GetNamePrefix() const;

		private:
			Mode m_Mode;
			std::vector< std::pair< ParamInfo, double > > m_Params;
			std::vector< std::pair< ParamInfo, double > >::iterator FindParamByName( const String& name );

			// current name prefixes
			std::vector< String > m_NamePrefixes;
		};

		class OPT_API Parameterizable
		{
		public:
			Parameterizable() { };
			virtual ~Parameterizable() { };

			virtual void ProcessParameters( opt::ParamSet& par ) = 0;
		};
	}
}

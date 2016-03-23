#pragma once

#include "scone/core/core.h"
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
			double Get( const String& name, const PropNode& props, const String node_name );
			double TryGet( const String& name, const PropNode& props, const String node_name, double default_value );
			double GetMeanStd( const String& name, double init_mean, double init_std, double min, double max );
			double GetMinMax( const String& name, double init_min, double init_max, double min, double max );

			bool CheckValues();
			void RestrainValues();
			void InitRandom();

			size_t GetParamCount() const { return m_Params.size(); }
			const ParamInfo& GetParamInfo( Index idx ) { return m_Params[ idx ].first; }

			void UpdateMeanStd( const std::vector< ParamSet >& parsets );
			void UpdateMeanStd( const std::vector< double >& means, const std::vector< double >& stds );

			size_t GetFreeParamCount();
			std::vector< double > GetFreeParamValues();
			void SetFreeParamValues( std::vector< double >& values );

			void SetMode( Mode m ) { m_Mode = m; }
			bool IsInConstructionMode() { return m_Mode == ConstructionMode; }

			void Write( const String& filename ) const;
			void Read( const String& filename );

			std::ostream& ToStream( std::ostream& str ) const;
			std::istream& FromStream( std::istream& str, bool log_results = false );

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

		class OPT_API ScopedParamSetPrefixer
		{
		public:
			ScopedParamSetPrefixer( ParamSet& par, const String& prefix ) : m_ParamSet( par ) { m_ParamSet.PushNamePrefix( prefix ); }
			~ScopedParamSetPrefixer() { m_ParamSet.PopNamePrefix(); }
		private:
			ParamSet& m_ParamSet;
		};

		class OPT_API Parameterizable
		{
		public:
			Parameterizable() { };
			virtual ~Parameterizable() { };

			virtual void ProcessParameters( opt::ParamSet& par ) = 0;
		};

		inline std::ostream& operator<<( std::ostream& str, const ParamSet& par ) { return par.ToStream( str ); }
		inline std::istream& operator>>( std::istream& str, ParamSet& par ) { return par.FromStream( str ); }
	}
}

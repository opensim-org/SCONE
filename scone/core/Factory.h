#pragma once

namespace scone
{
	template< typename F >
	struct Factory
	{
	public:
		F& GetCreateFunc( const String& name )
		{
			auto it = m_CreateFuncs.find( name );
			if ( it != m_CreateFuncs.end() )
				return it->second;
			else SCONE_THROW( "Unknown type: " + name );
		}

		template< typename C >
		void RegisterCreateFunc( const String& name )
		{
			m_CreateFuncs[ name ] = boost::factory< C* >();
		}

	private:
		std::map< String, F > m_CreateFuncs;
	};
}

#pragma once

#include "sim.h"
#include <vector>
#include "..\core\Exception.h"

namespace scone
{
	namespace sim
	{
		/// Simple struct to allow tree hierarchy
		class SCONE_SIM_API Link
		{
		public:
			Link( Body& b, Joint& j ) : m_Body( &b ), m_Joint( &j ) { };
			Link( Body& b ) : m_Body( &b ), m_Joint( nullptr ) { };

			String ToString( const String& prefix = "" );
			
			Body& body() { return *m_Body; }
			Joint& joint() { SCONE_ASSERT( m_Joint != nullptr ); return *m_Joint; }
			bool has_joint() { return m_Joint != nullptr; }
			std::vector< LinkUP >& children() { return m_Children; }

		private:
			Body* m_Body;
			Joint* m_Joint;
			std::vector< LinkUP > m_Children;

		private: // noncopyable
			Link( const Link& );
			Link& operator=( const Link& );
		};
	}
}

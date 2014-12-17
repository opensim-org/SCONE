#pragma once

#include "sim.h"
#include <vector>

namespace scone
{
	namespace sim
	{
		/// Simple struct to allow tree hierarchy
		class SCONE_SIM_API Link
		{
		public:
			Link( Body& b, Joint& j ) : body( b ), joint( j ) { };

			Body& body;
			Joint& joint;
			std::vector< LinkUP > children;

		private: // noncopyable
			Link( const Link& );
			Link& operator=( const Link& );
		};
	}
}

#pragma once

namespace scone
{
	namespace opt
	{
		class Optimization
		{
		public:
			Optimization() { };
			virtual ~Optimization() { };

			void Run( const String& config_file );
			
		protected:
		private:
		};
	}
}

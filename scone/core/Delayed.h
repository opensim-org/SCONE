#pragma once

namespace scone
{
	// TODO: move to core
	template< typename T >
	class CORE_API Delayed
	{
	public:
		Delayed() { };
		virtual ~Delayed() { };

		void SetCurrent( double time, const T& value )
		{
			//TODO
		}

		T GetDelayed()
		{
			//TODO
		}
			
	private:
		std::deque< std::pair< double, T > > m_Data;
	};
}

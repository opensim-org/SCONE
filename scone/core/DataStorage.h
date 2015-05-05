#pragma once

#include "core.h"
#include <map>
#include <vector>
#include <utility>

namespace scone
{
	class CORE_API DataStorage
	{
	public:
		typedef double ValueType;

		class Frame
		{
		public:
			Frame( const Frame& other );
			Frame& operator=( const Frame& other );
			Frame( DataStorage& store, TimeInSeconds time );
			void AddSample( const String& label, const ValueType& value );

			TimeInSeconds time;
			std::vector< ValueType > data;

		private:
			DataStorage& m_Store;
		};

		DataStorage();
		virtual ~DataStorage();

		Frame& AddFrame( TimeInSeconds time );
		size_t GetLabelIndex( const String& label );
		size_t GetLabelCount();
			
	private:
		std::map< String, size_t > m_Labels;
		std::vector< Frame > m_Frames;
	};
}

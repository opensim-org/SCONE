#pragma once

namespace scone
{
	class CORE_API HasData
	{
	public:
		HasData() { };
		virtual ~HasData() { };

		virtual void AddData( Storage::Frame& frame ) = 0;
	};
}

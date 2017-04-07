#pragma once

#include "scone/core/PropNode.h"
#include "scone/optimization/ParamSet.h"

namespace scone
{
	class SCONE_API Sensor
	{
	public:
		Sensor();
		Sensor( const PropNode& pn, ParamSet& par, class Model& model, const class Area& target_area );
		virtual ~Sensor();

		virtual Real GetValue() const { SCONE_THROW_NOT_IMPLEMENTED; }
		virtual String GetName() const = 0;

		// special case for multichannel sensors
		virtual Count GetChannelCount() { return 1; }
		virtual Real GetValue( Index idx ) const { SCONE_ASSERT( idx == 0 ); return GetValue(); }
	};
}

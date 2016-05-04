#pragma once

#include "sim.h"
#include "scone/core/PropNode.h"
#include "scone/opt/ParamSet.h"

namespace scone
{
	namespace sim
	{
		class SCONE_API Sensor
		{
		public:
			Sensor();
			Sensor( const PropNode& pn, opt::ParamSet& par, sim::Model& model, const Area& target_area );
			virtual ~Sensor();

			virtual Real GetValue() const { SCONE_THROW_NOT_IMPLEMENTED; }
			virtual String GetName() const = 0;

			// special case for multichannel sensors
			virtual Count GetChannelCount() { return 1; }
			virtual Real GetValue( Index idx ) const { SCONE_ASSERT( idx == 0 ); return GetValue(); }
		};
	}
}

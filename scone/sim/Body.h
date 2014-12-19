#pragma once

#include "sim.h"
#include "..\core\Vec3.h"
#include "..\core\Quat.h"
#include "..\core\Named.h"

namespace scone
{
	namespace sim
	{
		class SCONE_SIM_API Body : public Named
		{
		public:
			Body();
			virtual ~Body();

			virtual Vec3 GetPos() = 0;
			virtual Quat GetOri() = 0;

			virtual Vec3 GetLinVel() = 0;
			virtual Vec3 GetAngVel() = 0;
		};
	}
}

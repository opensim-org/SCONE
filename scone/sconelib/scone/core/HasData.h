#pragma once

#include "Storage.h"
#include "flut/flag_set.hpp"

namespace scone
{
	enum class StoreDataTypes {
		Unknown,
		State,
		MuscleActivation,
		MuscleExcitation,
		MuscleFiberProperties,
		GroundReactionForce,
		JointReactionForce,
		CenterOfMass,
		ControllerData
	};

	using StoreDataFlags = flut::flag_set< StoreDataTypes >;

	class SCONE_API HasData
	{
	public:
		virtual void StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) = 0;
		inline virtual ~HasData() {}
	};
}

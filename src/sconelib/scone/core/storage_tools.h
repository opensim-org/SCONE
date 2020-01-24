#pragma once

#include "Storage.h"
#include "scone/model/Side.h"

namespace scone
{
	SCONE_API Storage<> ExtractNormalized( const Storage<>& sto, TimeInSeconds begin, TimeInSeconds end );
	SCONE_API Storage<> ExtractGaitCycle( const Storage<>& sto, const String& force_channel, const Real threshold );
}

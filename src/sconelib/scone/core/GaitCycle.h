#pragma once

#include "scone/core/platform.h"
#include "scone/core/types.h"
#include "scone/model/Side.h"
#include "Storage.h"
#include "scone/core/string_tools.h"

namespace scone
{
	struct GaitCycle
	{
		Side side_ = NoSide;
		TimeInSeconds begin_ = 0;
		TimeInSeconds swing_ = 0;
		TimeInSeconds end_ = 0;
		Vec3 begin_pos_;
		Vec3 end_pos_;

		TimeInSeconds stance_duration() const { return swing_ - begin_; }
		TimeInSeconds swing_duration() const { return end_ - swing_; }
		TimeInSeconds duration() const { return end_ - begin_; }
		Real length() const { return xo::length( end_pos_ - begin_pos_ ); }
		Real velocity() const { return length() / duration(); }
	};

	SCONE_API std::vector<GaitCycle> ExtractGaitCycles( const Storage<>& sto, Real threshold, TimeInSeconds min_stance );
}

namespace xo
{
	SCONE_API string to_str( const scone::GaitCycle& c );
}

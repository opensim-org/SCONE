#pragma once

#include "spot/search_point.h"
#include "spot/objective_info.h"
#include "spot/par_tools.h"

namespace scone
{
	using Params = spot::par_io;
	using ParamInstance = spot::search_point;
	using ParamInfo = spot::objective_info;

	using ScopedParamSetPrefixer = spot::scoped_prefix;

	//class SCONE_API ScopedParamSetPrefixer
	//{
	//public:
	//	ScopedParamSetPrefixer( Params& par, const String& prefix ) : m_ParamSet( par ) { m_ParamSet.push_prefix( prefix ); }
	//	~ScopedParamSetPrefixer() { m_ParamSet.pop_prefix(); }
	//private:
	//	Params& m_ParamSet;
	//};
}

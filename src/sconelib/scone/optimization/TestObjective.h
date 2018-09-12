#pragma once

#include "Objective.h"

namespace scone
{
	class SCONE_API TestObjective : public Objective
	{
	public:
		TestObjective( const PropNode& pn );
		virtual ~TestObjective() {}
		
		virtual fitness_t evaluate( const SearchPoint& point ) const override;

	protected:
		size_t dim_;
		virtual String GetClassSignature() const override;

	};
}

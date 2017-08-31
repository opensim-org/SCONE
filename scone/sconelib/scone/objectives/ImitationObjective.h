#pragma once

#include "scone/optimization/Objective.h"
#include "scone/optimization/Params.h"
#include "scone/model/Simulation.h"
#include "scone/core/PropNode.h"

#include "Measure.h"
#include <vector>
#include "flut/system/path.hpp"
#include "../core/Storage.h"

namespace scone
{
	class SCONE_API ImitationObjective : public Objective
	{
	public:
		ImitationObjective( const PropNode& props );
		virtual ~ImitationObjective();

		virtual fitness_t evaluate( const ParamInstance& point ) const override;
		fitness_t EvaluateModel( Model& m ) const;

		path file;

	protected:
		virtual String GetClassSignature() const override;

	private:
		PropNode m_ModelPropsCopy;
		Storage<> m_Storage;
		String m_Signature;
		std::vector< Index > m_ExcitationChannels;
	};
}

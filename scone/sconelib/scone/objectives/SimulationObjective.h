#pragma once

#include "scone/optimization/Objective.h"
#include "scone/optimization/Params.h"
#include "scone/model/Simulation.h"
#include "scone/core/PropNode.h"

#include "Measure.h"
#include <vector>
#include "flut/system/path.hpp"

namespace scone
{
	class SCONE_API SimulationObjective : public Objective
	{
	public:
		SimulationObjective( const PropNode& props );
		virtual ~SimulationObjective();

		double max_duration;

		virtual fitness_t evaluate( const ParamInstance& point ) const override;
		fitness_t EvaluateModel( Model& m ) const;
		ModelUP CreateModelFromParameters( Params& par ) const;
		ModelUP CreateModelFromParFile( const path& parfile ) const;

	protected:
		virtual String GetClassSignature() const override;

	private:
		PropNode m_ModelPropsCopy;
		String m_Signature;
	};
}

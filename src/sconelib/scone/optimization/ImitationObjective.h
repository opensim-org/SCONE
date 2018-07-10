#pragma once

#include "scone/optimization/Objective.h"
#include "scone/optimization/Params.h"
#include "scone/model/Simulation.h"
#include "scone/core/PropNode.h"
#include "scone/measures/Measure.h"

#include <vector>
#include "xo/filesystem/path.h"
#include "scone/core/Storage.h"
#include "ModelObjective.h"

namespace scone
{
	class SCONE_API ImitationObjective : public ModelObjective
	{
	public:
		ImitationObjective( const PropNode& props );
		virtual ~ImitationObjective();

		virtual void AdvanceModel( Model& m, TimeInSeconds t ) const override;
		virtual TimeInSeconds GetDuration() const override { return m_Storage.Back().GetTime(); }
		virtual fitness_t GetResult( Model& m ) const override;
		virtual PropNode GetReport( Model& m ) const override;

		path file_;
		size_t frame_delta_;

	protected:
		virtual String GetClassSignature() const override;

	private:
		Storage<> m_Storage;
		String m_Signature;
		std::vector< index_t > m_ExcitationChannels;
		std::vector< index_t > m_SensorChannels;
	};
}

#pragma once

#include "scone/core/types.h"
#include "scone/core/PropNode.h"
#include "scone/optimization/ParamSet.h"
#include "scone/model/Model.h"
#include "scone/model/Area.h"

namespace scone
{
	class SCONE_API ReflexController : public sim::Controller
	{
	public:
		ReflexController( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& target_area );
		virtual ~ReflexController();

		virtual UpdateResult UpdateControls( sim::Model& model, double timestamp ) override;
		virtual String GetClassSignature() const override;

	private:
		std::vector< ReflexUP > m_Reflexes;

		ReflexController( const ReflexController& other );
		ReflexController operator=( const ReflexController& other );
	};
}

#pragma once

#include "scone/core/types.h"
#include "scone/core/PropNode.h"
#include "scone/optimization/ParamSet.h"
#include "scone/model/Model.h"
#include "scone/model/Area.h"

namespace scone
{
	class SCONE_API ReflexController : public Controller
	{
	public:
		ReflexController( const PropNode& props, ParamSet& par, Model& model, const Area& target_area );
		virtual ~ReflexController();

		virtual UpdateResult UpdateControls( Model& model, double timestamp ) override;
		virtual String GetClassSignature() const override;
		virtual void StoreData( Storage<Real>::Frame& frame ) override;

	private:
		std::vector< ReflexUP > m_Reflexes;

		ReflexController( const ReflexController& other );
		ReflexController operator=( const ReflexController& other );
	};
}

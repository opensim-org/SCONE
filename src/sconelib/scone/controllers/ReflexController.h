#pragma once

#include "scone/core/types.h"
#include "scone/core/PropNode.h"
#include "scone/optimization/Params.h"
#include "scone/model/Model.h"
#include "scone/model/Locality.h"

namespace scone
{
	// Class supporting control through reflexes.
	class ReflexController : public Controller
	{
	public:
		ReflexController( const PropNode& props, Params& par, Model& model, const Locality& target_area );
		virtual ~ReflexController();

		virtual bool ComputeControls( Model& model, double timestamp ) override;
		virtual String GetClassSignature() const override;
		virtual void StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const override;

	private:
		std::vector< ReflexUP > m_Reflexes;

		ReflexController( const ReflexController& other );
		ReflexController operator=( const ReflexController& other );
	};
}

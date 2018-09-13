#pragma once

#include "scone/core/types.h"
#include "scone/core/PropNode.h"
#include "scone/optimization/Params.h"
#include "scone/model/Model.h"
#include "scone/model/Locality.h"

namespace scone
{
	/// Controller based on reflexes -- see also Reflex and subclasses.
	class ReflexController : public Controller
	{
	public:
		ReflexController( const PropNode& props, Params& par, Model& model, const Locality& target_area );
		ReflexController( const ReflexController& other ) = delete;
		ReflexController operator=( const ReflexController& other ) = delete;
		virtual ~ReflexController();

		/// Indicate if reflexes should be the same for left and right; default = true.
		bool symmetric;

		/// Child node containing all reflexes.
		const PropNode* Reflexes;

		virtual bool ComputeControls( Model& model, double timestamp ) override;
		virtual String GetClassSignature() const override;
		virtual void StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const override;

	private:
		std::vector< ReflexUP > m_Reflexes;
	};
}

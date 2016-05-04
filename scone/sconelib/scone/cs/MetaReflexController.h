#pragma once

#include "scone/sim/Controller.h"
#include "cs.h"

namespace scone
{
	namespace cs
	{
		class SCONE_API MetaReflexController : public sim::Controller
		{
		public:
			MetaReflexController( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area );
			virtual ~MetaReflexController();

			virtual UpdateResult UpdateControls( sim::Model& model, double timestamp ) override;

			const std::vector< MetaReflexDofUP >& GetReflexDofs() const { return m_ReflexDofs; }
			const std::vector< MetaReflexVirtualMuscleUP >& GetVirtualMuscles() const { return m_VirtualMuscles; }

		private:
			MetaReflexController( const MetaReflexController& other );
			MetaReflexController& operator=( const MetaReflexController& other );

			virtual String GetClassSignature() const override;

			virtual void StoreData( Storage< Real >::Frame& frame ) override;

			std::vector< MetaReflexDofUP > m_ReflexDofs;
			std::vector< MetaReflexVirtualMuscleUP > m_VirtualMuscles;
			std::vector< MetaReflexMuscleUP > m_ReflexMuscles;
		};
	}
}

#pragma once

#include "cs.h"
#include "../sim/Controller.h"
#include "../core/Delayer.h"

namespace scone
{
	namespace cs
	{
		class CS_API ReflexController : public sim::Controller
		{
		public:
			ReflexController( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& target_area );
			virtual ~ReflexController();

			virtual UpdateResult UpdateControls( sim::Model& model, double timestamp ) override;
			virtual String GetMainSignature() const override;

		private:
			std::vector< ReflexUP > m_Reflexes;

			ReflexController( const ReflexController& other );
			ReflexController operator=( const ReflexController& other );
		};
	}
}

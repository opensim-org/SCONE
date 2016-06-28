#pragma once

#include "scone/sim/Model.h"
#include "scone/opt/ParamSet.h"
#include "scone/core/PropNode.h"

namespace scone
{
	namespace cs
	{
		class SCONE_API Reflex
		{
		public:
			Reflex( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area );
			virtual ~Reflex();

			virtual void ComputeControls( double timestamp );
			TimeInSeconds delay;

		protected:
			sim::Actuator& m_Target;
			static String GetReflexName( const String& target, const String& source );
		};
	}
}

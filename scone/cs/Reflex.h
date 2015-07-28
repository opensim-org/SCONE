#pragma once

#include "cs.h"
#include "../sim/Model.h"
#include "../opt/ParamSet.h"
#include "../core/PropNode.h"

namespace scone
{
	namespace cs
	{
		class CS_API Reflex
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

#pragma once

#include "cs.h"
#include "../sim/Controller.h"

namespace scone
{
	namespace cs
	{
		class CS_API ReflexController : public sim::Controller
		{
		public:
			ReflexController( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& target_area );
			virtual ~ReflexController();

			struct Reflex
			{
				size_t source_idx;
				size_t target_idx;
				
				// Reflex parameters
				Real delay;
				Real length_gain;
				Real length_ofs;
				Real force_gain;
				Real velocity_gain;

				// values

			};
			
		protected:
		private:
		};
	}
}

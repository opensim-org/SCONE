#pragma once

#include "Measure.h"
#include "..\core\Factory.h"
#include "../sim/Body.h"
#include <functional>

namespace scone
{
	namespace cs
	{
		class JumpingMeasure : public Measure, public Factoryable< sim::Controller, JumpingMeasure >
		{
		public:
			JumpingMeasure();
			virtual ~JumpingMeasure() { };

			virtual bool UpdateControls( sim::Model& model, double timestamp ) override;
			virtual double GetResult( sim::Model& model ) override;
			virtual void ProcessParameters( opt::ParamSet& par ) override;
			virtual void InitFromModel( sim::Model& model ) override;

			virtual void ProcessProperties( const PropNode& props ) override;

		private:
			String target_body;
			sim::Body* m_pTargetBody; // non-owning pointer
			double m_Initial;
			double m_Best;
			bool m_Upward;
		};
	}
}

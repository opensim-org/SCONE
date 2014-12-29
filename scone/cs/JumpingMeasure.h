#pragma once

#include "Measure.h"
#include "..\core\Factory.h"

namespace scone
{
	namespace cs
	{
		class JumpingMeasure : public Measure, public Factoryable< sim::Controller, JumpingMeasure >
		{
		public:
			JumpingMeasure() { };
			virtual ~JumpingMeasure() { };

			virtual bool UpdateControls( sim::Model& model, double timestamp ) override;
			virtual double GetResult( sim::Model& model ) override;
			virtual void ProcessParameters( opt::ParamSet& par ) override;

		private:
			double m_Initial;
			double GetCurrentComHeight( sim::Model& model );
		};
	}
}

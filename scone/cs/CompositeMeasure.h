#pragma once

#include "Measure.h"
#include "cs.h"
#include "../sim/sim.h"

namespace scone
{
	namespace cs
	{
		class CS_API CompositeMeasure : public Measure
		{
		public:
			CompositeMeasure( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area );
			virtual ~CompositeMeasure();

			virtual void UpdateControls( sim::Model& model, double timestamp ) override;
			virtual double GetResult( sim::Model& model ) override;
			virtual PropNode GetReport() override;

			struct Term
			{
				Term();;
				Term( const PropNode& pn );
				Term( Term&& other );;
				String name;
				double weight;
				double threshold;
				double offset;
				MeasureUP measure;
			private:
				Term( const Term& other );
			};

		protected:
			virtual String GetMainSignature() override;

		private:
			CompositeMeasure( CompositeMeasure& other );
			CompositeMeasure& operator=( CompositeMeasure& other );

			std::vector< Term > m_Terms;
			PropNode m_Report;
		};
	}
}

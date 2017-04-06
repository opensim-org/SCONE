#pragma once

#include "Measure.h"
#include "scone/core/types.h"

namespace scone
{
	class SCONE_API CompositeMeasure : public Measure
	{
	public:
		CompositeMeasure( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area );
		virtual ~CompositeMeasure();

		virtual UpdateResult UpdateAnalysis( const sim::Model& model, double timestamp ) override;
		virtual double GetResult( sim::Model& model ) override;

		struct Term
		{
			Term();
			Term( const PropNode& pn );
			Term( Term&& other );
			String name;
			double weight;
			double threshold;
			double offset;
			MeasureUP measure;
		private:
			Term( const Term& other );
		};

	protected:
		virtual String GetClassSignature() const override;

	private:
		CompositeMeasure( CompositeMeasure& other );
		CompositeMeasure& operator=( CompositeMeasure& other );

		virtual void StoreData( Storage< Real >::Frame& frame ) override;

		std::vector< Term > m_Terms;
		std::vector< MeasureUP > m_Measures;
	};
}

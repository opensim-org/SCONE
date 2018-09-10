#pragma once

#include "Measure.h"
#include "scone/core/types.h"

namespace scone
{
	class SCONE_API CompositeMeasure : public Measure
	{
	public:
		CompositeMeasure( const PropNode& props, Params& par, Model& model, const Locality& area );
		virtual ~CompositeMeasure();
		CompositeMeasure( CompositeMeasure& other ) = delete;
		CompositeMeasure& operator=( CompositeMeasure& other ) = delete;

		virtual bool UpdateMeasure( const Model& model, double timestamp ) override;
		virtual double GetResult( Model& model ) override;

		struct Term
		{
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

		virtual void StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const override;

	protected:
		virtual String GetClassSignature() const override;

	private:
		std::vector< Term > m_Terms;
		std::vector< MeasureUP > m_Measures;
	};
}

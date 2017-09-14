#pragma once
#include "Measure.h"
#include "scone/core/StringMap.h"
#include "scone/core/Statistic.h"
#include "scone/core/Vec3.h"

namespace scone
{
	class EffortMeasure : public Measure
	{
	public:
		EffortMeasure( const PropNode& props, Params& par, Model& model, const Locality& area );
		virtual ~EffortMeasure();

		enum EnergyMeasureType { UnknownMeasure, TotalForce, Wang2012, Constant, Uchida2016 };
		static StringMap< EnergyMeasureType > m_MeasureNames;

		virtual UpdateResult UpdateAnalysis( const Model& model, double timestamp ) override;
		virtual double GetResult( Model& model ) override;

		EnergyMeasureType measure_type;
		bool use_cost_of_transport;
		Real specific_tension;
		Real muscle_density;
		Real default_muscle_slow_twitch_ratio;
		bool use_symmetric_fiber_ratios;

	protected:
		virtual String GetClassSignature() const override;
		virtual void StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const override;

	private:
		Real m_Wang2012BasalEnergy;
		Real m_Uchida2016BasalEnergy;
		Real m_AerobicFactor;
		Statistic< double > m_Energy;
		Vec3 m_InitComPos;
		PropNode m_Report;
		std::vector< Real > m_SlowTwitchFiberRatios;
		struct MuscleProperties {
			MuscleProperties( const PropNode& props );
			String muscle;
			Real slow_twitch_ratio;
		};

		double GetEnergy( const Model& model ) const;
		double GetWang2012( const Model& model ) const;
		double GetUchida2016( const Model& model ) const;
		double GetTotalForce( const Model& model ) const;
		void SetSlowTwitchRatios( const PropNode& props, const Model& model );
	};
}

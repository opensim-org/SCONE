#pragma once
#include "Measure.h"
#include "scone/core/StringMap.h"
#include "scone/core/Statistic.h"
#include "scone/core/Vec3.h"

namespace scone
{
	// Class: EffortMeasure
	class EffortMeasure : public Measure
	{
	public:
		EffortMeasure( const PropNode& props, Params& par, Model& model, const Locality& area );
		virtual ~EffortMeasure();

		// Enum: EnergyMeasureType
		//   TotalForce - Total muscle force
		//   Wang2012 - Use metabolic energy measure as defined in [Wang et al. 2016]
		//   Uchida2016 - Use metabolic energy measure as defined in [Uchida et al. 2016]
		enum EnergyMeasureType { UnknownMeasure, TotalForce, Wang2012, Constant, Uchida2016 };

		// Prop: measure_type
		// Energy model to be used, can be: TotalForce, Wang2012, Uchida2016, or Constant
		EnergyMeasureType measure_type; 

		// Prop: use_cost_of_transport
		// Flag indicating to use (energy / distance) as a result
		bool use_cost_of_transport = false;

		/// Value to use for specific tension
		Real specific_tension = 0.1;

		/// Value to use for muscle density
		Real muscle_density; 

		/// slow-twitch ratio, if not defined per-muscle
		Real default_muscle_slow_twitch_ratio;

		/// fiber ratios are the same for left and right
		bool use_symmetric_fiber_ratios;

		/// Minimum distance used for cost of transport computation
		Real min_distance;

		virtual bool UpdateMeasure( const Model& model, double timestamp ) override;
		virtual double GetResult( Model& model ) override;

	protected:
		virtual String GetClassSignature() const override;
		virtual void StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const override;

	private:
		Real m_Wang2012BasalEnergy;
		Real m_Uchida2016BasalEnergy;
		Real m_AerobicFactor;
		Statistic< double > m_Energy;
		static StringMap< EnergyMeasureType > m_MeasureNames;
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

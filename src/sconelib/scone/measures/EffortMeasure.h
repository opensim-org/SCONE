/*
** EffortMeasure.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once
#include "Measure.h"
#include "scone/core/StringMap.h"
#include "scone/core/Statistic.h"
#include "scone/core/Vec3.h"

namespace scone
{
	/// Measures the energy consumption of a model during simulation, according to various models.
	/// Can also be used to minimize cost-of-transfer, i.e. energy / distance.
	class EffortMeasure : public Measure
	{
	public:
		EffortMeasure( const PropNode& props, Params& par, Model& model, const Location& loc );
		virtual ~EffortMeasure();

		/// Model for measuring energy
		enum EnergyMeasureType {
			UnknownMeasure,
			TotalForce, ///< Total muscle force.
			Wang2012, ///< Use metabolic energy measure as defined in [Wang et al. 2012].
			Constant, ///< Constant energy.
			Uchida2016 ///< Use metabolic energy measure as defined in [Uchida et al. 2016].
		};

		/// Energy model to be used, can be: TotalForce, Wang2012, Uchida2016, or Constant; default = UnknownMeasure.
		EnergyMeasureType measure_type; 

		/// Flag indicating to use (energy / distance) as a result; default = 0.
		bool use_cost_of_transport;

		/// Value to use for specific tension; default = 0.25e6.
		Real specific_tension;

		/// Value to use for muscle density; default = 1059.7.
		Real muscle_density; 

		/// Default slow / twitch ratio, if not defined per muscle; default = 0.5.
		Real default_muscle_slow_twitch_ratio;

		/// Indicate if fiber ratios are the same for left and right; default = true.
		bool use_symmetric_fiber_ratios;

		/// Minimum distance used for cost of transport computation; default = 1.0.
		Real min_distance;

		virtual bool UpdateMeasure( const Model& model, double timestamp ) override;
		virtual double ComputeResult( Model& model ) override;

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

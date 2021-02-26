/*
** EffortMeasure.h
**
** Copyright (C) 2013-2021 Thomas Geijtenbeek and contributors. All rights reserved.
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
	/** Supported effort models:
		- ''Constant'': constant energy measure
		- ''TotalForce'': total muscle force
		- ''Wang2012'': metabolic energy measure as defined in [Wang et al. 2012]
		- ''Uchida2016'': metabolic energy measure as defined in [Uchida et al. 2012]
		- ''SquaredMuscleStress'': summed squared muscle stress: (force / PCSA)^2
		- ''CubedMuscleStress'': summed squared muscle stress: (force / PCSA)^3
		- ''SquaredMuscleActivation'': summed squared muscle activation: (activation)^2
		- ''CubedMuscleActivation'': summed cubed muscle activation: (activation)^3

		This can also be used for cost-of-transport, using the ''use_cost_of_transport'' parameter.
	*/
	class EffortMeasure : public Measure
	{
	public:
		EffortMeasure( const PropNode& props, Params& par, const Model& model, const Location& loc );

		/// Model for measuring energy
		enum EnergyMeasureType {
			UnknownMeasure,
			Constant, ///< Constant energy.
			TotalForce, ///< Total muscle force.
			Wang2012, ///< Use metabolic energy measure as defined in [Wang et al. 2012].
			Uchida2016, ///< Use metabolic energy measure as defined in [Uchida et al. 2016].
			SquaredMuscleStress, ///< Use the summed squared muscle stress as a measure
			CubedMuscleStress, ///< Use the summed squared muscle stress as a measure
			SquaredMuscleActivation, ///< Use the summed squared muscle muscle activation
			CubedMuscleActivation ///< Use the summed cubed muscle muscle activation
		};

		/// Energy model to be used, can be: ''TotalForce'', ''Wang2012'', ''Uchida2016'', or ''Constant''; default = ''UnknownMeasure''.
		EnergyMeasureType measure_type;

		/// Flag indicating to use (energy / distance) as a result; default = 0.
		bool use_cost_of_transport;

		/// Value to use for specific tension; default = 0.25e6.
		Real specific_tension;

		/// Value to use for muscle density; default = 1059.7.
		Real muscle_density;

		/// Default slow / twitch ratio if not defined per muscle (used by Uchida2016); default = 0.5.
		Real default_muscle_slow_twitch_ratio;

		/// Indicate if fiber ratios are the same for left and right (used by Uchida2016); default = true.
		bool use_symmetric_fiber_ratios;

		/// Minimum distance used for cost of transport computation; default = 1.0.
		Real min_distance;

		/// Divide result by number of muscles, useful for muscle activation measures; default = false.
		bool use_average_per_muscle;

		virtual bool UpdateMeasure( const Model& model, double timestamp ) override;
		virtual double ComputeResult( const Model& model ) override;

	protected:
		virtual String GetClassSignature() const override;
		virtual void StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const override;

	private:
		Real m_Wang2012BasalEnergy;
		Real m_Uchida2016BasalEnergy;
		Real m_AerobicFactor;
		Statistic< double > m_Effort;
		static StringMap< EnergyMeasureType > m_MeasureNames;
		Vec3 m_InitComPos;
		PropNode m_Report;
		std::vector< Real > m_SlowTwitchFiberRatios;
		struct MuscleProperties {
			MuscleProperties( const PropNode& props );
			String muscle;
			Real slow_twitch_ratio;
		};

		double GetCurrentEffort( const Model& model ) const;
		double GetWang2012( const Model& model ) const;
		double GetUchida2016( const Model& model ) const;
		double GetTotalForce( const Model& model ) const;
		void SetSlowTwitchRatios( const PropNode& props, const Model& model );
		double GetSquaredMuscleStress( const Model& model ) const;
		double GetCubedMuscleStress( const Model& model ) const;
		double GetSquaredMuscleActivation( const Model& model ) const;
		double GetCubedMuscleActivation( const Model& model ) const;
	};
}

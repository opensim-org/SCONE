#include "stdafx.h"
#include "EnergyMeasure.h"
#include "boost/foreach.hpp"
#include "../sim/Model.h"
#include "../sim/sim.h"
#include "../sim/Muscle.h"

#include <boost/assign.hpp>

namespace scone
{
	namespace cs
	{
		EnumStringMap< EnergyMeasure::EnergyMeasureType > m_MeasureNames = EnumStringMap< EnergyMeasure::EnergyMeasureType >(
			EnergyMeasure::TotalForce, "TotalForce",
			EnergyMeasure::Umberger2010, "Umberger2010"
			);

		EnergyMeasure::EnergyMeasure( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area ) :
		Measure( props, par, model, area )
		{

		}

		EnergyMeasure::~EnergyMeasure()
		{

		}

		void EnergyMeasure::UpdateControls( sim::Model& model, double timestamp )
		{
			double s = GetTotalForce( model );

			m_Energy.AddSample( s, timestamp );
		}

		double EnergyMeasure::GetResult( sim::Model& model )
		{
			return m_Energy.GetTotal();
		}

		double EnergyMeasure::GetTotalForce( sim::Model& model )
		{
			double f = 1.0; // base muscle force
			BOOST_FOREACH( sim::MuscleUP& mus, model.GetMuscles() )
				f += mus->GetForce();

			return f;
		}

		scone::String EnergyMeasure::GetSignature()
		{
			switch( m_Type )
			{
			case TotalForce: return "TF";
			case Umberger2010: return "U";
			default: return "Unknown";
			}
		}

	}
}

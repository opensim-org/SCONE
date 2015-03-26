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
		//typename EnumStringMap< EnergyMeasure::EnergyMeasureType > m_MeasureNames = boost::assign::map_list_of
		//	( EnergyMeasure::UnknownMeasure, "Unknown" );

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
}
}

#pragma once

#include "sim_simbody.h"
#include "../../core/Vec3.h"

#include <OpenSim/OpenSim.h>
#include "../../core/ResourceCache.h"

namespace scone
{
	// opensim model resource cache
	template <> OpenSim::Model* ResourceCache< OpenSim::Model >::CreateFirst( const String& name )
	{
		return new OpenSim::Model( name );
	}

	template <> OpenSim::Storage* ResourceCache< OpenSim::Storage >::CreateFirst( const String& name )
	{
		return new OpenSim::Storage( name );
	}

	namespace sim
	{
		inline Vec3 ToVec3( const SimTK::Vec3& vec ) { return Vec3( vec[ 0 ], vec[ 1 ], vec[ 2 ] ); }
	}

	namespace sim
	{
		template< typename T >
		T* SetupEnergyConsumptionProbe( OpenSim::Model& model, T* probe )
		{
			model.addProbe( probe );
			Real total_mus_mass = 0.0;
			// add all muscles to the probe
			for ( int idx = 0; idx < model.getMuscles().getSize(); ++idx )
			{
				OpenSim::Muscle& mus = model.getMuscles().get( idx );
				//double mass = mus.getOptimalFiberLength() * mus.getMaxIsometricForce() / 23500.0; // Derived from [Wang2012]
				double mass = ( mus.getMaxIsometricForce() / 0.25e6 ) * 1059.7 * mus.getOptimalFiberLength(); // Derived from OpenSim doxygen
				total_mus_mass += mass;
				probe->addMuscle( mus.getName(), 0.5 );
			}

			probe->setOperation("integrate");

			//std::cout << "Total muscle mass: " << total_mus_mass << std::endl;

			return probe;
		}
	}
}
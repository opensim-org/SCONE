#pragma once

#include "OpenSim/Simulation/Model/Force.h"
#include "OpenSim/Common/Property.h"

namespace OpenSim
{
	class ConstantForce : public Force
	{
		OpenSim_DECLARE_CONCRETE_OBJECT( ConstantForce, Force );
	public:
		OpenSim_DECLARE_OPTIONAL_PROPERTY( body, std::string,
			"Name of Body to which this actuator is applied." );
		OpenSim_DECLARE_PROPERTY( point, SimTK::Vec3,
			"Location of application point; in body frame unless point_is_global=true" );
		OpenSim_DECLARE_PROPERTY( point_is_global, bool,
			"Interpret point in Ground frame if true; otherwise, body frame." );
		OpenSim_DECLARE_PROPERTY( force, SimTK::Vec3,
			"Force application; in body frame unless force_is_global=true." );
		OpenSim_DECLARE_PROPERTY( force_is_global, bool,
			"Interpret direction in Ground frame if true; otherwise, body frame." );
		OpenSim_DECLARE_PROPERTY( torque, SimTK::Vec3,
			"Force application; in body frame unless torque_is_global=true." );
		OpenSim_DECLARE_PROPERTY( torque_is_global, bool,
			"Interpret direction in Ground frame if true; otherwise, body frame." );

		//==============================================================================
		// PUBLIC METHODS
		//==============================================================================
			/** Default constructor or construct with body name given. An empty
			name ("") is treated as though it were unspecified. **/
		ConstantForce( const std::string& bodyName = "" );

		// Uses default (compiler-generated) destructor, copy constructor, copy 
		// assignment operator.

	private:
		void setNull();
		void constructProperties();

		//--------------------------------------------------------------------------
		// Implement Force interface
		//--------------------------------------------------------------------------
		void computeForce( const SimTK::State& state,
			SimTK::Vector_<SimTK::SpatialVec>& bodyForces,
			SimTK::Vector& mobilityForces ) const OVERRIDE_11;

		//--------------------------------------------------------------------------
		// Implement ModelComponent interface
		//--------------------------------------------------------------------------
		// Setup method to initialize Body reference
		void connectToModel( Model& model ) OVERRIDE_11;

		//--------------------------------------------------------------------------
		// Implement Object interface.
		//--------------------------------------------------------------------------
		void updateFromXMLNode( SimTK::Xml::Element& node, int versionNumber = -1 ) OVERRIDE_11;

		// Corresponding Body to which the point actuator is applied.
		SimTK::ReferencePtr<Body> _body;
	};
}

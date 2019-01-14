/*
** ConstantForce.h
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include <OpenSim/Simulation/Model/Force.h>
#include <OpenSim/Common/Property.h>

namespace OpenSim
{
	class ConstantForce : public Force
	{
		OpenSim_DECLARE_CONCRETE_OBJECT( ConstantForce, Force );
	public:
		OpenSim_DECLARE_OPTIONAL_PROPERTY( body, std::string,
			"Name of Body to which this actuator is applied." );
		OpenSim_DECLARE_PROPERTY( point_is_global, bool,
			"Interpret point in Ground frame if true; otherwise, body frame (default=false)" );
		OpenSim_DECLARE_PROPERTY( force_is_global, bool,
			"Interpret direction in Ground frame if true; otherwise, body frame (default=true)" );
		OpenSim_DECLARE_PROPERTY( torque_is_global, bool,
			"Interpret direction in Ground frame if true; otherwise, body frame (default=false)" );

		//==============================================================================
		// PUBLIC METHODS
		//==============================================================================
			/** Default constructor or construct with body name given. An empty
			name ("") is treated as though it were unspecified. **/
		ConstantForce( const std::string& bodyName = "" );
		// Uses default (compiler-generated) destructor, copy constructor, copy 
		// assignment operator.

		void setNull();
		void setForce( const SimTK::Vec3& force );
		void setPoint( const SimTK::Vec3& point );
		void setForceAtPoint( const SimTK::Vec3& force, const SimTK::Vec3& point );
		void setTorque( const SimTK::Vec3& torque );

		SimTK::Vec3& getForce() { return force_; }
		SimTK::Vec3& getPoint() { return point_; }
		SimTK::Vec3& getTorque() { return torque_; }

		const std::vector< std::string >& GetStateVariableNames() { return state_variable_names_; }

	private:
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
		SimTK::ReferencePtr<Body> body_;
		SimTK::Vec3 force_;
		SimTK::Vec3 point_;
		SimTK::Vec3 torque_;
		std::vector< std::string > state_variable_names_;
	};
}

#pragma once

#include "OpenSim/Simulation/Model/Force.h"
#include "OpenSim/Common/Property.h"
#include "OpenSim/Common/Object.h"

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
			"Interpret direction in Ground frame if true; otherwise, body frame (default=false)" );
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
		void setForceAtPoint( const SimTK::Vec3& force, const SimTK::Vec3& point );
		void setTorque( const SimTK::Vec3& torque );

		const SimTK::Vec3 getForce();
		const SimTK::Vec3 getPoint();
		const SimTK::Vec3 getTorque();

		virtual SimTK::Vector computeStateVariableDerivatives( const SimTK::State& s ) const override;

	protected:
		virtual void addToSystem( SimTK::MultibodySystem& system ) const override;
		virtual void connectToModel( Model& model ) override;
		virtual void initStateFromProperties( SimTK::State& state ) const override;
		virtual void setPropertiesFromState( const SimTK::State& state ) override;

	private:
		void constructProperties();

		//--------------------------------------------------------------------------
		// Implement Force interface
		//--------------------------------------------------------------------------
		void computeForce( const SimTK::State& state,
			SimTK::Vector_<SimTK::SpatialVec>& bodyForces,
			SimTK::Vector& mobilityForces ) const OVERRIDE_11;

		//--------------------------------------------------------------------------
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

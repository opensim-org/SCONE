#include "ConstantForce.h"
#include "SimTKcommon/SmallMatrix.h"
#include "OpenSim/Simulation/SimbodyEngine/SimbodyEngine.h"
#include "OpenSim/Simulation/Model/Model.h"
#include "OpenSim/Simulation/Model/BodySet.h"

using SimTK::Vec3;

namespace OpenSim
{
	ConstantForce::ConstantForce( const std::string& bodyName )
	{
		setNull();
		constructProperties();

		if ( !bodyName.empty() )
			set_body( bodyName );
	}

	void ConstantForce::setNull()
	{
		setAuthors( "Thomas Geijtenbeek" );
	}

	void ConstantForce::constructProperties()
	{
		constructProperty_body();
		constructProperty_point( Vec3( 0 ) );
		constructProperty_point_is_global( false );
		constructProperty_force( Vec3( 0, 0, 0 ) );
		constructProperty_force_is_global( false );
		constructProperty_torque( Vec3( 0, 0, 0 ) );
		constructProperty_torque_is_global( false );
	}

	void ConstantForce::computeForce( const SimTK::State& s, SimTK::Vector_<SimTK::SpatialVec>& bodyForces, SimTK::Vector& mobilityForces ) const
	{
		const SimbodyEngine& engine = getModel().getSimbodyEngine();

		if ( !_model || !_body ) return;

		// apply force
		Vec3 forceVec = get_force();
		Vec3 lpoint = get_point();
		if ( !get_force_is_global() )
			engine.transform( s, *_body, forceVec, engine.getGroundBody(), forceVec );
		if ( get_point_is_global() )
			engine.transformPosition( s, engine.getGroundBody(), lpoint, *_body, lpoint );
		applyForceToPoint( s, *_body, lpoint, forceVec, bodyForces );

		// apply torque
		Vec3 torqueVec = get_torque();
		if ( !get_torque_is_global() )
			engine.transform( s, *_body, forceVec, engine.getGroundBody(), torqueVec );
		applyTorque( s, *_body, torqueVec, bodyForces );
	}

	void ConstantForce::connectToModel( Model& model )
	{
		Super::connectToModel( model );

		const std::string& bodyName = get_body();
		if ( !model.updBodySet().contains( bodyName ) )
			throw OpenSim::Exception( "PointActuator: Unknown body (" + bodyName + ") specified in Actuator " + getName() );

		_body = &model.updBodySet().get( bodyName );
	}

	void ConstantForce::updateFromXMLNode( SimTK::Xml::Element& node, int versionNumber )
	{
		Super::updateFromXMLNode( node, versionNumber );

		if ( _model && !get_body().empty() )
		{
			const std::string& bodyName = get_body();
			_body = &_model->updBodySet().get( bodyName );
		}
	}
}

#include "ConstantForce.h"
#include "SimTKcommon/SmallMatrix.h"
#include "OpenSim/Simulation/SimbodyEngine/SimbodyEngine.h"
#include "OpenSim/Simulation/Model/Model.h"
#include "OpenSim/Simulation/Model/BodySet.h"
#include "scone/core/Log.h"

using SimTK::Vec3;

namespace OpenSim
{
	ConstantForce::ConstantForce( const std::string& bodyName )
	{
		constructProperties();
		setNull();
		setAuthors( "Thomas Geijtenbeek" );

		if ( !bodyName.empty() )
			set_body( bodyName );

		state_variable_names_.push_back( bodyName + "_force_x" );
		state_variable_names_.push_back( bodyName + "_force_y" );
		state_variable_names_.push_back( bodyName + "_force_z" );
		state_variable_names_.push_back( bodyName + "_point_x" );
		state_variable_names_.push_back( bodyName + "_point_y" );
		state_variable_names_.push_back( bodyName + "_point_z" );
		state_variable_names_.push_back( bodyName + "_torque_x" );
		state_variable_names_.push_back( bodyName + "_torque_y" );
		state_variable_names_.push_back( bodyName + "_torque_z" );
	}

	void ConstantForce::setNull()
	{
		setForceAtPoint( SimTK::Vec3( 0 ), SimTK::Vec3( 0 ) );
		setTorque( SimTK::Vec3( 0 ) );
	}

	void ConstantForce::setForceAtPoint( const SimTK::Vec3& force, const SimTK::Vec3& point )
	{
		force_ = force;
		point_ = point;
	}

	void ConstantForce::setTorque( const SimTK::Vec3& torque )
	{
		torque_ = torque;
	}

	const SimTK::Vec3 ConstantForce::getForce()
	{
		SimTK::Vec3 force;
		for ( int i = 0; i < 3; ++i )
			force[ i ] = getStateVariable( _model->getWorkingState(), state_variable_names_[ i ] );
		return force;
	}

	const SimTK::Vec3 ConstantForce::getPoint()
	{
		SimTK::Vec3 point;
		for ( int i = 0; i < 3; ++i )
			point[ i ] = getStateVariable( _model->getWorkingState(), state_variable_names_[ 3 + i ] );
		return point;
	}

	const SimTK::Vec3 ConstantForce::getTorque()
	{
		SimTK::Vec3 torque;
		for ( int i = 0; i < 3; ++i )
			torque[ i ] = getStateVariable( _model->getWorkingState(), state_variable_names_[ 6 + i ] );
		return torque;
	}

	void ConstantForce::initStateFromProperties( SimTK::State& state ) const
	{
		Super::initStateFromProperties( state );
		for ( int i = 0; i < 3; ++i )
		{
			setStateVariable( state, state_variable_names_[ i ], force_[ i ] );
			setStateVariable( state, state_variable_names_[ 3 + i ], point_[ i ] );
			setStateVariable( state, state_variable_names_[ 6 + i ], torque_[ i ] );
		}
	}

	void ConstantForce::setPropertiesFromState( const SimTK::State& state )
	{
		Super::setPropertiesFromState( state );
		for ( int i = 0; i < 3; ++i )
		{
			force_[ i ] = getStateVariable( state, state_variable_names_[ i ] );
			point_[ i ] = getStateVariable( state, state_variable_names_[ 3 + i ] );
			torque_[ i ] = getStateVariable( state, state_variable_names_[ 6 + i ] );
		}
	}

	void ConstantForce::constructProperties()
	{
		constructProperty_body();
		constructProperty_point_is_global( false );
		constructProperty_force_is_global( false );
		constructProperty_torque_is_global( false );
	}

	void ConstantForce::computeForce( const SimTK::State& s, SimTK::Vector_<SimTK::SpatialVec>& bodyForces, SimTK::Vector& mobilityForces ) const
	{
		const SimbodyEngine& engine = getModel().getSimbodyEngine();
		if ( !_model || !body_ ) return;

		// apply force
		Vec3 forceVec = force_;
		Vec3 lpoint = point_;
		if ( !get_force_is_global() )
			engine.transform( s, *body_, forceVec, engine.getGroundBody(), forceVec );
		if ( get_point_is_global() )
			engine.transformPosition( s, engine.getGroundBody(), lpoint, *body_, lpoint );
		applyForceToPoint( s, *body_, lpoint, forceVec, bodyForces );

		// apply torque
		Vec3 torqueVec = torque_;
		if ( !get_torque_is_global() )
			engine.transform( s, *body_, forceVec, engine.getGroundBody(), torqueVec );
		applyTorque( s, *body_, torqueVec, bodyForces );
	}

	void ConstantForce::connectToModel( Model& model )
	{
		Super::connectToModel( model );

		const std::string& bodyName = get_body();
		if ( !model.updBodySet().contains( bodyName ) )
			throw OpenSim::Exception( "ConstantForce: Unknown body (" + bodyName + ") specified in Actuator " + getName() );

		body_ = &model.updBodySet().get( bodyName );
	}

	SimTK::Vector ConstantForce::computeStateVariableDerivatives( const SimTK::State& s ) const
	{
		SimTK::Vector derivs = Super::computeStateVariableDerivatives( s );
		const int n = derivs.size();
		derivs.resizeKeep( n + (int)state_variable_names_.size() );
		const double dt = 0.01;
		for ( int i = 0; i < 3; ++i )
		{
			derivs[ n + i ] = ( force_[ i ] - getStateVariable( s, state_variable_names_[ i ] ) ) / dt;
			derivs[ n + 3 + i ] = ( point_[ i ] - getStateVariable( s, state_variable_names_[ 3 + i ] ) ) / dt;
			derivs[ n + 6 + i ] = ( torque_[ i ] - getStateVariable( s, state_variable_names_[ 6 + i ] ) ) / dt;
		}
		return derivs;
	}

	void ConstantForce::updateFromXMLNode( SimTK::Xml::Element& node, int versionNumber )
	{
		Super::updateFromXMLNode( node, versionNumber );

		if ( _model && !get_body().empty() )
		{
			const std::string& bodyName = get_body();
			body_ = &_model->updBodySet().get( bodyName );
		}
	}

	void ConstantForce::addToSystem( SimTK::MultibodySystem& system ) const
	{
		Super::addToSystem( system );

		for ( auto& s : state_variable_names_ )
			addStateVariable( s );
	}
}

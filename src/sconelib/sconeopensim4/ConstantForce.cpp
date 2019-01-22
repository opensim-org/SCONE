/*
** ConstantForce.cpp
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "ConstantForce.h"

#include "SimTKcommon/SmallMatrix.h"
#include "OpenSim/Simulation/SimbodyEngine/SimbodyEngine.h"
#include "OpenSim/Simulation/Model/Model.h"
#include "OpenSim/Simulation/Model/BodySet.h"

#include "xo/system/log.h"

using SimTK::Vec3;

namespace OpenSim
{
	ConstantForce::ConstantForce( const std::string& bodyName )
	{
		constructProperties();
		setNull();
		setAuthors( "Thomas Geijtenbeek" );

		state_variable_names_.reserve( 9 );
		state_variable_names_.push_back( bodyName + "_force_x" );
		state_variable_names_.push_back( bodyName + "_force_y" );
		state_variable_names_.push_back( bodyName + "_force_z" );
		state_variable_names_.push_back( bodyName + "_point_x" );
		state_variable_names_.push_back( bodyName + "_point_y" );
		state_variable_names_.push_back( bodyName + "_point_z" );
		state_variable_names_.push_back( bodyName + "_torque_x" );
		state_variable_names_.push_back( bodyName + "_torque_y" );
		state_variable_names_.push_back( bodyName + "_torque_z" );

		if ( !bodyName.empty() )
			set_body( bodyName );
	}

	void ConstantForce::setNull()
	{
		setForceAtPoint( SimTK::Vec3( 0 ), SimTK::Vec3( 0 ) );
		setTorque( SimTK::Vec3( 0 ) );
	}

	void ConstantForce::setForce( const SimTK::Vec3& force )
	{
		force_ = force;
	}

	void ConstantForce::setForceAtPoint( const SimTK::Vec3& force, const SimTK::Vec3& point )
	{
		force_ = force;
		point_ = point;
	}

	void ConstantForce::setPoint( const SimTK::Vec3& point )
	{
		point_ = point;
	}

	void ConstantForce::setTorque( const SimTK::Vec3& torque )
	{
		torque_ = torque;
	}

	void ConstantForce::constructProperties()
	{
		constructProperty_body();
		constructProperty_point_is_global( false );
		constructProperty_force_is_global( true );
		constructProperty_torque_is_global( false );
	}

	void ConstantForce::computeForce( const SimTK::State& s, SimTK::Vector_<SimTK::SpatialVec>& bodyForces, SimTK::Vector& mobilityForces ) const
	{
		const SimbodyEngine& engine = getModel().getSimbodyEngine();

		if ( !_model || !body_ ) return;

		const auto& ground = getModel().getGround();

		// apply force
		Vec3 forceVec = force_;
		Vec3 lpoint = point_;
		if ( !get_force_is_global() ) {
			engine.transform( s, *body_, forceVec, ground, forceVec );
		}
		if ( get_point_is_global() ) {
			engine.transformPosition( s, ground, lpoint, *body_, lpoint );
		}
		applyForceToPoint( s, *body_, lpoint, forceVec, bodyForces );

		// apply torque
		Vec3 torqueVec = torque_;
		if ( !get_torque_is_global() )
			engine.transform( s, *body_, torqueVec, ground, torqueVec );
		applyTorque( s, *body_, torqueVec, bodyForces );
	}

	void ConstantForce::extendConnectToModel( Model& model )
	{
		Super::extendConnectToModel( model );

		const std::string& bodyName = get_body();
		if ( !model.updBodySet().contains( bodyName ) )
			throw OpenSim::Exception( "PointActuator: Unknown body (" + bodyName + ") specified in Actuator " + getName() );

		body_ = &model.updBodySet().get( bodyName );
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
}

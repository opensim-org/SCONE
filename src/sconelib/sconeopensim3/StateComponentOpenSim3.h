/*
** StateComponentOpenSim3.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include <OpenSim/Simulation/Model/ModelComponent.h>
#include "scone/model/StateComponent.h"
// #include "scone/core/HasData.h"

namespace OpenSim
{
	class StateComponentOpenSim3 : public OpenSim::ModelComponent//, public virtual HasData
	{
		OpenSim_DECLARE_CONCRETE_OBJECT(StateComponentOpenSim3, ModelComponent);
	public:
		/// Takes ownership of the pointer.
		StateComponentOpenSim3( scone::StateComponent* stateComponent );
		~StateComponentOpenSim3() { delete m_stateComponent; };

		/// Extract the state of this component from global state.
		std::vector< scone::Real > getStateVariables( const SimTK::State& s ) const;
		/// Extract the state of this component from global state.
		void setStateVariables( SimTK::State& s, std::vector< scone::Real > x0 ) const;
		/// Calculate the state derivative.
		SimTK::Vector computeStateVariableDerivatives( const SimTK::State& s ) const override;
		/// Initializes the state from properties
		void initStateFromProperties( SimTK::State& s ) const override;
		/// Add state variables to the system.
		void addToSystem( SimTK::MultibodySystem& system ) const override;
		// /// From HasData.
		// void StoreData( scone::Storage< scone::Real >::Frame& frame,
		// 				const scone::StoreDataFlags& flags ) const override;
	private:
		scone::StateComponent* m_stateComponent;
		std::vector< scone::String > m_stateVariables;
	};
}

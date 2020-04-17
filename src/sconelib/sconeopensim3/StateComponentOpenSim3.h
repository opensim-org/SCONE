/*
** StateComponentOpenSim3.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include <OpenSim/Simulation/Model/ModelComponent.h>
#include "SimTKcommon/internal/ReferencePtr.h"
#include "scone/model/StateComponent.h"

namespace OpenSim
{
	/// Transforms a scone::StateComponent, that implements a
	/// differential equation (continuous or hybrid), into an
	/// OpenSim::ModelComponent so that the differential equation can
	/// be integrated numerically by OpenSim.
	class StateComponentOpenSim3 : public OpenSim::ModelComponent
	{
	private:
		OpenSim_DECLARE_CONCRETE_OBJECT(StateComponentOpenSim3, ModelComponent);
		/// A class that introduces event detection and handling for
		/// hybrid systems.
		class EventHandler : public SimTK::TriggeredEventHandler
		{
		public:
			EventHandler(StateComponentOpenSim3*, scone::StateComponent*);
			SimTK::Real getValue(const SimTK::State& s) const override;
			void handleEvent(SimTK::State& s,
							 SimTK::Real accuracy,
							 bool& shouldTerminate) const override;
		private:
			SimTK::ReferencePtr<StateComponentOpenSim3> m_modelComponent;
			SimTK::ReferencePtr<scone::StateComponent> m_stateComponent;
		};

	public:
		/// Takes ownership of the StateComponent pointer.
		StateComponentOpenSim3( scone::StateComponent* stateComponent );
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

	private:
		SimTK::ReferencePtr<scone::StateComponent> m_stateComponent;
		std::vector< scone::String > m_stateVariables;
	};
}

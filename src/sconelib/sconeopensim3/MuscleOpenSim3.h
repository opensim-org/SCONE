/*
** MuscleOpenSim3.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "platform.h"
#include "scone/model/Muscle.h"
#include "xo/container/circular_deque.h"

#include <vector>
#include "xo/container/flat_map.h"

namespace OpenSim
{
	class Muscle;
}

namespace scone
{
	class ModelOpenSim3;

	class SCONE_OPENSIM_3_API MuscleOpenSim3 : public Muscle
	{
	public:
		MuscleOpenSim3( ModelOpenSim3& model, OpenSim::Muscle& mus );
		virtual ~MuscleOpenSim3();

		// access to bodies
		virtual const Body& GetOriginBody() const override;
		virtual const Body& GetInsertionBody() const override;
		virtual const Model& GetModel() const override;

		// muscle parameters
		virtual Real GetMaxIsometricForce() const override;
		virtual Real GetOptimalFiberLength() const override;
		virtual Real GetTendonSlackLength() const override;

		// current force / length / velocity
		virtual Real GetForce() const override;
		virtual Real GetNormalizedForce() const override;

		virtual Real GetLength() const override;
		virtual Real GetVelocity() const override;

		virtual Real GetFiberForce() const override;
		virtual Real GetNormalizedFiberForce() const override;
		virtual Real GetActiveFiberForce() const override;

		virtual Real GetFiberLength() const override;
		virtual Real GetNormalizedFiberLength() const override;

		virtual Real GetFiberVelocity() const override;
		virtual Real GetNormalizedFiberVelocity() const override;

		virtual Real GetTendonLength() const override;

		virtual Real GetActiveForceLengthMultipler() const override;
		virtual Real GetMaxContractionVelocity() const override;

		virtual Real GetActivation() const override;
		virtual Real GetExcitation() const override;

		virtual std::vector< Vec3 > GetMusclePath() const override;
		virtual void SetExcitation( Real u ) override;

		OpenSim::Muscle& GetOsMuscle() { return m_osMus; }

		virtual const String& GetName() const override;
		virtual Real GetMomentArm( const Dof& dof ) const override;

	private:
		ModelOpenSim3& m_Model;
		OpenSim::Muscle& m_osMus;
		mutable xo::flat_map< const Dof*, Real > m_MomentArmCache;
	};
}

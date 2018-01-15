#pragma once

#include "sim_simbody.h"
#include "scone/model/Muscle.h"
#include "xo/container/circular_deque.h"

#include <vector>

namespace OpenSim
{
	class Muscle;
}

namespace scone
{
	class Model_Simbody;

	class SCONE_API Muscle_Simbody : public Muscle
	{
	public:
		Muscle_Simbody( Model_Simbody& model, OpenSim::Muscle& mus );
		virtual ~Muscle_Simbody();

		// access to bodies
		virtual const Link& GetOriginLink() const override;
		virtual const Link& GetInsertionLink() const override;
		virtual const Model& GetModel() const override;

		// muscle parameters
		virtual Real GetMaxIsometricForce() const override;
		virtual Real GetOptimalFiberLength() const override;
		virtual Real GetTendonSlackLength() const override;
		virtual Real GetMass( Real specific_tension, Real muscle_density ) const override;

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
		OpenSim::Muscle& m_osMus;
		Model_Simbody& m_Model;
		mutable xo::flat_map< const Dof*, Real > m_MomentArmCache;
	};
}

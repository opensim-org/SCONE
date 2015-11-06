#include "stdafx.h"
#include "Muscle_Simbody.h"
#include "Model_Simbody.h"

#include "../../core/Exception.h"
#include "../../core/Profiler.h"
#include "Dof_Simbody.h"

namespace scone
{
	namespace sim
	{
		Muscle_Simbody::Muscle_Simbody( Model_Simbody& model, OpenSim::Muscle& mus ) : m_Model( model ), m_osMus( mus )
		{
		}

		Muscle_Simbody::~Muscle_Simbody()
		{

		}

		const String& Muscle_Simbody::GetName() const
		{
			return m_osMus.getName();
		}

		scone::Real Muscle_Simbody::GetOptimalFiberLength() const
		{
			return m_osMus.getOptimalFiberLength();
		}

		scone::Real Muscle_Simbody::GetTendonSlackLength() const
		{
			return m_osMus.getTendonSlackLength();
		}

		scone::Real Muscle_Simbody::GetMass( Real specific_tension, Real muscle_density) const
		{
			return ( GetMaxIsometricForce() / specific_tension ) * muscle_density * GetOptimalFiberLength(); // from OpenSim Umberger metabolic energy model docs
		}

		scone::Real Muscle_Simbody::GetForce() const
		{
			SCONE_PROFILE_SCOPE;
			// OpenSim: why can't I just use getWorkingState()?
			// OpenSim: why must I update to Dynamics for getForce()?
			m_Model.GetOsimModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Dynamics );
			return m_osMus.getForce( m_Model.GetTkState() );
		}

		scone::Real Muscle_Simbody::GetNormalizedForce() const 
		{
			SCONE_PROFILE_SCOPE;
			return GetForce() / GetMaxIsometricForce();
		}

		scone::Real scone::sim::Muscle_Simbody::GetLength() const
		{
			SCONE_PROFILE_SCOPE;
			m_Model.GetOsimModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Position );
			return m_osMus.getLength( m_Model.GetTkState() );
		}

		scone::Real Muscle_Simbody::GetVelocity() const
		{
			SCONE_PROFILE_SCOPE;
			m_Model.GetOsimModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Velocity );
			return m_osMus.getLengtheningSpeed( m_Model.GetTkState() );
		}

		scone::Real Muscle_Simbody::GetFiberForce() const
		{
			SCONE_PROFILE_SCOPE;
			return m_osMus.getFiberForce( m_Model.GetTkState() );
		}

		scone::Real Muscle_Simbody::GetNormalizedFiberForce() const 
		{
			SCONE_PROFILE_SCOPE;
			return m_osMus.getFiberForce( m_Model.GetTkState() ) / m_osMus.getMaxIsometricForce();
		}

		scone::Real scone::sim::Muscle_Simbody::GetFiberLength() const
		{
			SCONE_PROFILE_SCOPE;
			return m_osMus.getFiberLength( m_Model.GetTkState() );
		}

		scone::Real Muscle_Simbody::GetNormalizedFiberLength() const 
		{
			SCONE_PROFILE_SCOPE;
			m_Model.GetOsimModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Position );
			return m_osMus.getNormalizedFiberLength( m_Model.GetTkState() );
		}

		scone::Real Muscle_Simbody::GetFiberVelocity() const
		{
			SCONE_PROFILE_SCOPE;
			return m_osMus.getFiberVelocity( m_Model.GetTkState() );
		}

		scone::Real Muscle_Simbody::GetNormalizedFiberVelocity() const 
		{
			SCONE_PROFILE_SCOPE;
			return m_osMus.getNormalizedFiberVelocity( m_Model.GetTkState() );
		}

		const Link& Muscle_Simbody::GetOriginLink() const 
		{
			auto& pps = m_osMus.getGeometryPath().getPathPointSet();
			return m_Model.FindLink( pps.get( 0 ).getBodyName() );
		}

		const Link& Muscle_Simbody::GetInsertionLink() const 
		{
			auto& pps = m_osMus.getGeometryPath().getPathPointSet();
			return m_Model.FindLink( pps.get( pps.getSize() - 1 ).getBodyName() );
		}

		scone::Real Muscle_Simbody::GetMomentArm( const Dof& dof ) const 
		{
			const Dof_Simbody& dof_sb = dynamic_cast< const Dof_Simbody& >( dof );
			return m_osMus.getGeometryPath().computeMomentArm( m_Model.GetTkState(), dof_sb.GetOsCoordinate() );
		}

		scone::Real scone::sim::Muscle_Simbody::GetTendonLength() const
		{
			return m_osMus.getTendonLength( m_Model.GetTkState() );
		}
		
		scone::Real scone::sim::Muscle_Simbody::GetMaxIsometricForce() const
		{
			return m_osMus.getMaxIsometricForce();
		}
		
		std::vector< Vec3 > scone::sim::Muscle_Simbody::GetMusclePath() const
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}
		
		scone::Real scone::sim::Muscle_Simbody::GetActivation() const
		{
			SCONE_PROFILE_SCOPE;
			return m_osMus.getActivation( m_Model.GetTkState() );
		}
		
		scone::Real scone::sim::Muscle_Simbody::GetExcitation() const
		{
			// use our own control value, as OpenSim calls getControls()
			// this could lead to infinite recursion
			return GetControlValue();
		}
		
		void scone::sim::Muscle_Simbody::SetExcitation( Real u )
		{
			m_osMus.setExcitation( m_Model.GetTkState(), u );
		}
	}
}

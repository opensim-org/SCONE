#include "stdafx.h"
#include "Muscle_Simbody.h"
#include "../../core/Exception.h"
#include "Model_Simbody.h"

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

		scone::Real Muscle_Simbody::GetForce()
		{
			// OpenSim: why can't I just use getWorkingState()?
			// OpenSim: why must I update to Dynamics for getForce()?
			m_Model.GetOsimModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Dynamics );
			return m_osMus.getForce( m_Model.GetTkState() );
		}

		scone::Real scone::sim::Muscle_Simbody::GetLength()
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}
		
		scone::Real scone::sim::Muscle_Simbody::GetFiberLength()
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}
		
		scone::Real scone::sim::Muscle_Simbody::GetTendonLength()
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}
		
		scone::Real scone::sim::Muscle_Simbody::GetMaxIsometricForce()
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}
		
		std::vector< Vec3 > scone::sim::Muscle_Simbody::GetMusclePath()
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}
		
		scone::Real scone::sim::Muscle_Simbody::GetActivation()
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}
		
		scone::Real scone::sim::Muscle_Simbody::GetExcitation()
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}
		
		void scone::sim::Muscle_Simbody::SetExcitation(Real u)
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}
}
}

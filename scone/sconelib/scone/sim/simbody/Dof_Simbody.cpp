#include "Dof_Simbody.h"
#include "Model_Simbody.h"
#include "Joint_Simbody.h"

#include <OpenSim/OpenSim.h>
#include "scone/core/Log.h"

namespace scone
{
	namespace sim
	{
		Dof_Simbody::Dof_Simbody( class Model_Simbody& model, OpenSim::Coordinate& coord ) :
		Dof( *FindByName( model.GetJoints(), coord.getJoint().getName() ) ),
		m_Model( model ),
		m_osCoord( coord ),
		m_pOsLimitForce( nullptr )
		{
			// find corresponding CoordinateLimitForce
			auto& forceSet = model.GetOsimModel().getForceSet();
			for ( int idx = 0; idx < forceSet.getSize(); ++idx )
			{
				// OpenSim: Set<T>::get( idx ) is const but returns non-const reference, is this a bug?
				const OpenSim::CoordinateLimitForce* clf = dynamic_cast< const OpenSim::CoordinateLimitForce* >( &forceSet.get( idx ) );
				if ( clf && clf->getProperty_coordinate().getValue() == coord.getName() )
				{
					// we have found a match!
					log::Trace( "Found limit force " + clf->getName() + " for coord " + coord.getName() );
					m_pOsLimitForce = clf;
					break; 
				}
			}
		}

		Dof_Simbody::~Dof_Simbody() { }

		scone::Real Dof_Simbody::GetPos() const
		{
			return m_osCoord.getValue( m_Model.GetTkState() );
		}

		scone::Real Dof_Simbody::GetVel() const
		{
			return m_osCoord.getSpeedValue( m_Model.GetTkState() );
		}

		const String& Dof_Simbody::GetName() const 
		{
			return m_osCoord.getName();
		}

		scone::Real Dof_Simbody::GetLimitForce() const
		{
			if ( m_pOsLimitForce )
				return m_pOsLimitForce->calcLimitForce( m_Model.GetTkState() );
			else return 0.0;
		}

		void Dof_Simbody::SetPos( Real pos, bool enforce_constraints )
		{
			if ( !m_osCoord.getLocked( m_Model.GetTkState() ) )
				m_osCoord.setValue( m_Model.GetTkState(), pos, enforce_constraints );
		}

		void Dof_Simbody::SetVel( Real vel )
		{
			if ( !m_osCoord.getLocked( m_Model.GetTkState() ) )
				m_osCoord.setSpeedValue( m_Model.GetTkState(), vel );
		}

		Vec3 Dof_Simbody::GetRotationAxis() const
		{
			return m_RotationAxis;
		}
	}
}

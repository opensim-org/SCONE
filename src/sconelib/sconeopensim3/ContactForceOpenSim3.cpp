#include "ContactForceOpenSim3.h"

#include "OpenSim/Simulation/Model/HuntCrossleyForce.h"
#include "ModelOpenSim3.h"
#include "scone/model/model_tools.h"
#include <variant>
#include "xo/geometry/quat.h"
#include "scone/core/Log.h"

namespace scone
{
	ContactForceOpenSim3::ContactForceOpenSim3( ModelOpenSim3& model, const OpenSim::HuntCrossleyForce& osForce ) :
		m_osForce( osForce ),
		m_Model( model ),
		m_LastNumDynamicsRealizations( -1 )
	{
		auto& osForceNonConst = const_cast<OpenSim::HuntCrossleyForce&>( m_osForce ); // hack for OpenSim bug
		auto& osGeometry = osForceNonConst.getContactParametersSet().get( 0 ).getGeometry();
		for ( int idx = 0; idx < osGeometry.size(); ++idx )
		{
			auto& cg = *FindByName( model.GetContactGeometries(), osGeometry.getValue( idx ) );
			if ( auto p = std::get_if< xo::plane >( &cg.GetShape() ) )
			{
				// initialize plane normal / pos, needed for cop computation
				m_PlaneNormal = cg.GetOri() * Vec3( p->normal_ );
				m_PlaneLocation = cg.GetPos();
			}
			m_Geometries.push_back( &cg );
		}

		auto labels = m_osForce.getRecordLabels();
		m_Labels.reserve( labels.size() );
		for ( int i = 0; i < labels.size(); ++i )
			m_Labels.push_back( labels[ i ] );
		m_Values.resize( labels.size() );

		// attach contact force to bodies
		for ( auto& b : model.GetBodies() )
		{
			for ( auto& cg : m_Geometries )
				if ( &cg->GetBody() == b.get() )
					dynamic_cast<BodyOpenSim3&>( *b ).AttachContactForce( this );
		}
	}

	ContactForceOpenSim3::~ContactForceOpenSim3()
	{}

	const String& ContactForceOpenSim3::GetName() const
	{
		return m_osForce.getName();
	}

	const Vec3& ContactForceOpenSim3::GetForce() const
	{
		UpdateForceValues();
		return m_Force;
	}

	const Vec3& ContactForceOpenSim3::GetMoment() const
	{
		UpdateForceValues();
		return m_Moment;
	}

	const Vec3& ContactForceOpenSim3::GetPoint() const
	{
		UpdateForceValues();
		return m_Point;
	}

	std::tuple<const Vec3&, const Vec3&, const Vec3&> ContactForceOpenSim3::GetForceMomentPoint() const
	{
		UpdateForceValues();
		return { m_Force, m_Moment, m_Point };
	}

	void ContactForceOpenSim3::UpdateForceValues() const
	{
		auto& osModel = m_Model.GetOsimModel();
		auto& tkState = m_Model.GetTkState();

		// realize the state and check the number of realizations
		// IMPORTANT: we use the getNumRealizationsOfThisStage() instead of time or step
		// because FixTkState() calls this multiple times before integration
		osModel.getMultibodySystem().realize( tkState, SimTK::Stage::Dynamics );
		int num_dyn = osModel.getMultibodySystem().getNumRealizationsOfThisStage( SimTK::Stage::Dynamics );

		// update m_ContactForceValues only if needed (performance)
		if ( m_LastNumDynamicsRealizations != num_dyn )
		{
			OpenSim::Array<double> forces = m_osForce.getRecordValues( tkState );
			for ( int i = 0; i < forces.size(); ++i )
				m_Values[ i ] = forces[ i ];
			m_LastNumDynamicsRealizations = num_dyn;

			m_Force.set( -m_Values[ 0 ], -m_Values[ 1 ], -m_Values[ 2 ] );
			m_Moment.set( -m_Values[ 3 ], -m_Values[ 4 ], -m_Values[ 5 ] );
			m_Point = GetPlaneCop( m_PlaneNormal, m_PlaneLocation, m_Force, m_Moment );
		}
	}
}
